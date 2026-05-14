#include "VulkanTexture2D.h"
#define STB_IMAGE_IMPLEMENTATION
#include "VulkanContext.h"
#include "VulkanUtils.h"
#include "Core/Core.h"

#include <stb_image.h>

namespace JuicyEngine
{
VulkanTexture2D::VulkanTexture2D(const std::string& Path)
{
    int TexChannels;
    stbi_uc* Pixels = stbi_load(Path.c_str(), &Width, &Height, &TexChannels, STBI_rgb_alpha);
    int ImageSize = Width * Height * 4;
    JE_CORE_ASSERT(Pixels, "Failed to load texture image!")

    const auto* Context = dynamic_cast<VulkanContext*>(VulkanContext::Get());

    VkBuffer StagingBuffer;
    VkDeviceMemory StagingBufferMemory;
    RHI::Vulkan::VulkanUtils::CreateBuffer(ImageSize,
                                           VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                           StagingBuffer,
                                           StagingBufferMemory);

    void* Data;
    vkMapMemory(Context->GetDevice()->GetLogicalDevice(), StagingBufferMemory, 0, ImageSize, 0, &Data);
    memcpy(Data, Pixels, static_cast<size_t>(ImageSize));
    vkUnmapMemory(Context->GetDevice()->GetLogicalDevice(), StagingBufferMemory);

    stbi_image_free(Pixels);
    Image.reset(new VulkanImage(Width,
                                Height,
                                VK_FORMAT_R8G8B8A8_SRGB,
                                VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT));

    Image->TransitionLayoutImmediate(VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    UploadData(StagingBuffer);
    Image->TransitionLayoutImmediate(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    vkDestroyBuffer(Context->GetDevice()->GetLogicalDevice(), StagingBuffer, nullptr);
    vkFreeMemory(Context->GetDevice()->GetLogicalDevice(), StagingBufferMemory, nullptr);

    CreateTextureSampler();
}

VulkanTexture2D::~VulkanTexture2D()
{
    const auto* Context = dynamic_cast<VulkanContext*>(VulkanContext::Get());

    vkDestroySampler(Context->GetDevice()->GetLogicalDevice(), TextureSampler, nullptr);
}

void VulkanTexture2D::CreateTextureSampler()
{
    const auto* Context = dynamic_cast<VulkanContext*>(VulkanContext::Get());
    VkPhysicalDeviceProperties Properties {};
    vkGetPhysicalDeviceProperties(Context->GetDevice()->GetPhysicalDevice(), &Properties);

    VkSamplerCreateInfo SamplerInfo {};
    SamplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    SamplerInfo.magFilter = VK_FILTER_LINEAR;
    SamplerInfo.minFilter = VK_FILTER_LINEAR;
    SamplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    SamplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    SamplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    SamplerInfo.anisotropyEnable = VK_TRUE;
    SamplerInfo.maxAnisotropy = Properties.limits.maxSamplerAnisotropy;
    SamplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    SamplerInfo.unnormalizedCoordinates = VK_FALSE;
    SamplerInfo.compareEnable = VK_FALSE;
    SamplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    SamplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

    auto Result = vkCreateSampler(Context->GetDevice()->GetLogicalDevice(), &SamplerInfo, nullptr, &TextureSampler);
    JE_CORE_ASSERT(Result == VK_SUCCESS, "Failed to create texture sampler!")
}

void VulkanTexture2D::UploadData(VkBuffer Buffer)
{
    VkCommandBuffer CommandBuffer = RHI::Vulkan::VulkanUtils::BeginSingleTimeCommands();

    VkBufferImageCopy Region {};
    Region.bufferOffset = 0;
    Region.bufferRowLength = 0;
    Region.bufferImageHeight = 0;
    Region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    Region.imageSubresource.mipLevel = 0;
    Region.imageSubresource.baseArrayLayer = 0;
    Region.imageSubresource.layerCount = 1;
    Region.imageOffset = {0, 0, 0};
    Region.imageExtent = {.width = (uint32_t) Width, .height = (uint32_t) Height, .depth = 1};

    vkCmdCopyBufferToImage(CommandBuffer, Buffer, Image->GetImage(), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &Region);

    RHI::Vulkan::VulkanUtils::EndSingleTimeCommands(CommandBuffer);
}

VkSampler& VulkanTexture2D::GetSampler()
{
    return TextureSampler;
}

VulkanImage* VulkanTexture2D::GetImage()
{
    return Image.get();
}
} // namespace JuicyEngine