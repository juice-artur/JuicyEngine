#include "jepch.h"

#include "VulkanImage.h"
#include "VulkanContext.h"
#include "VulkanUtils.h"
#include "Core/Core.h"

namespace JuicyEngine
{
VulkanImage::VulkanImage(
    uint32_t Width, uint32_t Height, VkFormat Format, VkImageUsageFlags Usage, VkMemoryPropertyFlags Props)
    : Width(Width), Height(Height), Format(Format)
{
    const auto* Context = dynamic_cast<VulkanContext*>(VulkanContext::Get());
    auto Device = Context->GetDevice()->GetLogicalDevice();

    CreateRawImage(Device, Width, Height, Format, VK_IMAGE_TILING_OPTIMAL, Usage, Props, Image, Memory);

    VkImageAspectFlags Aspect = (Format >= VK_FORMAT_D16_UNORM && Format <= VK_FORMAT_D32_SFLOAT_S8_UINT)
                                    ? VK_IMAGE_ASPECT_DEPTH_BIT
                                    : VK_IMAGE_ASPECT_COLOR_BIT;
    ImageView = CreateRawImageView(Image, Format, Aspect);
}

VulkanImage::~VulkanImage()
{
    const auto* Context = dynamic_cast<VulkanContext*>(VulkanContext::Get());

    vkDestroyImageView(Context->GetDevice()->GetLogicalDevice(), ImageView, nullptr);
    vkDestroyImage(Context->GetDevice()->GetLogicalDevice(), Image, nullptr);
    vkFreeMemory(Context->GetDevice()->GetLogicalDevice(), Memory, nullptr);
}

VkImage VulkanImage::GetImage() const
{
    return Image;
}

VkImageView VulkanImage::GetImageView() const
{
    return ImageView;
}

VkFormat VulkanImage::GetFormat() const
{
    return Format;
}

VkExtent2D VulkanImage::GetExtent() const
{
    return VkExtent2D {Width, Height};
}
void VulkanImage::TransitionLayout(VkCommandBuffer Cmd,
                                   VkImageLayout OldLayout,
                                   VkImageLayout NewLayout,
                                   VkImageAspectFlags Aspect)
{
    VkImageMemoryBarrier Barrier = {};
    Barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    Barrier.oldLayout = OldLayout;
    Barrier.newLayout = NewLayout;
    Barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    Barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    Barrier.image = Image;
    Barrier.subresourceRange.aspectMask = Aspect;
    Barrier.subresourceRange.baseMipLevel = 0;
    Barrier.subresourceRange.levelCount = 1;
    Barrier.subresourceRange.baseArrayLayer = 0;
    Barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags SourceStage;
    VkPipelineStageFlags DestinationStage;
    if (OldLayout == VK_IMAGE_LAYOUT_UNDEFINED && NewLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
    {
        Barrier.srcAccessMask = 0;
        Barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        SourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        DestinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (OldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && NewLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
    {
        Barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        Barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        SourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        DestinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else if (OldLayout == VK_IMAGE_LAYOUT_UNDEFINED && NewLayout == VK_IMAGE_LAYOUT_GENERAL)
    {
        Barrier.srcAccessMask = 0;
        Barrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_SHADER_READ_BIT;

        SourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        DestinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else
    {
        JE_CORE_ASSERT(false, "Unknown Layout Change!")
        return;
    }

    vkCmdPipelineBarrier(Cmd, SourceStage, DestinationStage, 0, 0, nullptr, 0, nullptr, 1, &Barrier);
}
void VulkanImage::TransitionLayoutImmediate(VkImageLayout OldLayout, VkImageLayout NewLayout, VkImageAspectFlags Aspect)
{
    VkCommandBuffer Cmd = RHI::Vulkan::VulkanUtils::BeginSingleTimeCommands();
    TransitionLayout(Cmd, OldLayout, NewLayout, Aspect);
    RHI::Vulkan::VulkanUtils::EndSingleTimeCommands(Cmd);
}

void VulkanImage::CreateRawImage(VkDevice Device,
                                 uint32_t Width,
                                 uint32_t Height,
                                 VkFormat Format,
                                 VkImageTiling Tiling,
                                 VkImageUsageFlags Usage,
                                 VkMemoryPropertyFlags Properties,
                                 VkImage& OutImage,
                                 VkDeviceMemory& OutImageMemory)
{
    VkImageCreateInfo ImageInfo {};
    ImageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    ImageInfo.imageType = VK_IMAGE_TYPE_2D;
    ImageInfo.extent.width = Width;
    ImageInfo.extent.height = Height;
    ImageInfo.extent.depth = 1;
    ImageInfo.mipLevels = 1;
    ImageInfo.arrayLayers = 1;
    ImageInfo.format = Format;
    ImageInfo.tiling = Tiling;
    ImageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    ImageInfo.usage = Usage;
    ImageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    ImageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    auto Result = vkCreateImage(Device, &ImageInfo, nullptr, &OutImage);
    JE_CORE_ASSERT(Result == VK_SUCCESS, "Failed to create image!")

    VkMemoryRequirements MemRequirements;
    vkGetImageMemoryRequirements(Device, OutImage, &MemRequirements);

    VkMemoryAllocateInfo AllocInfo {};
    AllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    AllocInfo.allocationSize = MemRequirements.size;
    AllocInfo.memoryTypeIndex = RHI::Vulkan::VulkanUtils::FindMemoryType(MemRequirements.memoryTypeBits, Properties);

    auto AllocateResult = vkAllocateMemory(Device, &AllocInfo, nullptr, &OutImageMemory);
    JE_CORE_ASSERT(AllocateResult == VK_SUCCESS, "Failed to allocate memory!")
    
    VkResult BindResult = vkBindImageMemory(Device, OutImage, OutImageMemory, 0);
    JE_CORE_ASSERT(BindResult == VK_SUCCESS, "Failed to bind image memory!")
}

VkImageView VulkanImage::CreateRawImageView(VkImage Image, VkFormat Format, VkImageAspectFlags AspectFlags)
{
    VkImageViewCreateInfo ViewInfo {};
    ViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    ViewInfo.image = Image;
    ViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    ViewInfo.format = Format;
    ViewInfo.subresourceRange.aspectMask = AspectFlags;
    ViewInfo.subresourceRange.baseMipLevel = 0;
    ViewInfo.subresourceRange.levelCount = 1;
    ViewInfo.subresourceRange.baseArrayLayer = 0;
    ViewInfo.subresourceRange.layerCount = 1;

    VkImageView ImageViewData;
    const auto* Context = dynamic_cast<VulkanContext*>(VulkanContext::Get());

    auto Result = vkCreateImageView(Context->GetDevice()->GetLogicalDevice(), &ViewInfo, nullptr, &ImageViewData);
    JE_CORE_ASSERT(Result == VK_SUCCESS, "Failed to create image view!")

    return ImageViewData;
}

}