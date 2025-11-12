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
		RHI::Vulkan::VulkanUtils::CreateBuffer(ImageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, StagingBuffer, StagingBufferMemory);

		void* Data;
		vkMapMemory(Context->GetDevice()->GetLogicalDevice(), StagingBufferMemory, 0, ImageSize, 0, &Data);
		memcpy(Data, Pixels, static_cast<size_t>(ImageSize));
		vkUnmapMemory(Context->GetDevice()->GetLogicalDevice(), StagingBufferMemory);

		stbi_image_free(Pixels);
		Format = VK_FORMAT_R8G8B8A8_SRGB;
		
		CreateImage(VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		TransitionImageLayout( VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		UploadData(StagingBuffer);
		TransitionImageLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		
		vkDestroyBuffer(Context->GetDevice()->GetLogicalDevice(), StagingBuffer, nullptr);
		vkFreeMemory(Context->GetDevice()->GetLogicalDevice(), StagingBufferMemory, nullptr);
		
		ImageView = CreateImageView(Image, Format, VK_IMAGE_ASPECT_COLOR_BIT);
		CreateTextureSampler();
	}
	
	VulkanTexture2D::~VulkanTexture2D()
	{
		const auto* Context = dynamic_cast<VulkanContext*>(VulkanContext::Get());

		vkDestroySampler(Context->GetDevice()->GetLogicalDevice(), TextureSampler, nullptr);
		vkDestroyImageView(Context->GetDevice()->GetLogicalDevice(), ImageView, nullptr);
		vkDestroyImage(Context->GetDevice()->GetLogicalDevice(), Image, nullptr);
		vkFreeMemory(Context->GetDevice()->GetLogicalDevice(), ImageMemory, nullptr);
	}
	
	void VulkanTexture2D::CreateImage(VkImageTiling Tiling,
	                                  VkImageUsageFlags Usage,
	                                  VkMemoryPropertyFlags Properties)
	{
		CreateImage(Width, Height, Format, Tiling, Usage, Properties, Image, ImageMemory);
	}

	void VulkanTexture2D::CreateTextureSampler()
	{
		const auto* Context = dynamic_cast<VulkanContext*>(VulkanContext::Get());
		VkPhysicalDeviceProperties Properties{};
		vkGetPhysicalDeviceProperties(Context->GetDevice()->GetPhysicalDevice(), &Properties);

		VkSamplerCreateInfo SamplerInfo{};
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

	void VulkanTexture2D::TransitionImageLayout(VkImageLayout OldLayout, VkImageLayout NewLayout)
	{
		VkCommandBuffer CommandBuffer = RHI::Vulkan::VulkanUtils::BeginSingleTimeCommands();
		
		VkImageMemoryBarrier Barrier = {};
		Barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		Barrier.oldLayout = OldLayout;
		Barrier.newLayout = NewLayout;
		Barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		Barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		Barrier.image = Image;
		Barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		Barrier.subresourceRange.baseMipLevel = 0;
		Barrier.subresourceRange.levelCount = 1;
		Barrier.subresourceRange.baseArrayLayer = 0;
		Barrier.subresourceRange.layerCount = 1;

		VkPipelineStageFlags SourceStage;
		VkPipelineStageFlags DestinationStage;
		if (OldLayout == VK_IMAGE_LAYOUT_UNDEFINED && NewLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
			Barrier.srcAccessMask = 0;
			Barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			SourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			DestinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		} else if (OldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && NewLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
			Barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			Barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			SourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			DestinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else
		{
			JE_CORE_ASSERT(false, "Unknown Layout Change!")
		}

		vkCmdPipelineBarrier(
			CommandBuffer,
			SourceStage, DestinationStage,
			0,
			0, nullptr,
			0, nullptr,
			1, &Barrier
		);
		
		RHI::Vulkan::VulkanUtils::EndSingleTimeCommands(CommandBuffer);
	}
	
	void VulkanTexture2D::UploadData(VkBuffer Buffer)
	{
		VkCommandBuffer CommandBuffer =  RHI::Vulkan::VulkanUtils::BeginSingleTimeCommands();

		VkBufferImageCopy Region{};
		Region.bufferOffset = 0;
		Region.bufferRowLength = 0;
		Region.bufferImageHeight = 0;
		Region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		Region.imageSubresource.mipLevel = 0;
		Region.imageSubresource.baseArrayLayer = 0;
		Region.imageSubresource.layerCount = 1;
		Region.imageOffset = {0, 0, 0};
		Region.imageExtent =
		{
			.width = (uint32_t)Width,
			.height = (uint32_t)Height,
			.depth = 1
		};

		vkCmdCopyBufferToImage(CommandBuffer, Buffer, Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &Region);

		 RHI::Vulkan::VulkanUtils::EndSingleTimeCommands(CommandBuffer);
	}
	VkImageView& VulkanTexture2D::GetImageView()
	{
		return ImageView;
	}
	VkSampler& VulkanTexture2D::GetSampler()
	{
		return TextureSampler;
	}
	void VulkanTexture2D::CreateImage(uint32_t Width,
	                                  uint32_t Height,
	                                  VkFormat Format,
	                                  VkImageTiling Tiling,
	                                  VkImageUsageFlags Usage,
	                                  VkMemoryPropertyFlags Properties,
	                                  VkImage& Image,
	                                  VkDeviceMemory& ImageMemory)
	{
		VkImageCreateInfo ImageInfo{};
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

		const auto* Context = dynamic_cast<VulkanContext*>(VulkanContext::Get());
		
		auto Result = vkCreateImage(Context->GetDevice()->GetLogicalDevice(), &ImageInfo, nullptr, &Image);
		JE_CORE_ASSERT(Result == VK_SUCCESS, "Failed to create image!")
		
		VkMemoryRequirements MemRequirements;
		vkGetImageMemoryRequirements(Context->GetDevice()->GetLogicalDevice(), Image, &MemRequirements);

		VkMemoryAllocateInfo AllocInfo{};
		AllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		AllocInfo.allocationSize = MemRequirements.size;
		AllocInfo.memoryTypeIndex = RHI::Vulkan::VulkanUtils::FindMemoryType(MemRequirements.memoryTypeBits, Properties);

		auto AllocateResult = vkAllocateMemory(Context->GetDevice()->GetLogicalDevice(), &AllocInfo, nullptr, &ImageMemory);
		JE_CORE_ASSERT(AllocateResult == VK_SUCCESS, "Failed to allocate memory!")

		vkBindImageMemory(Context->GetDevice()->GetLogicalDevice(), Image, ImageMemory, 0);
	}
	VkImageView VulkanTexture2D::CreateImageView(VkImage Image, VkFormat Format, VkImageAspectFlags AspectFlags)
	{
		VkImageViewCreateInfo ViewInfo{};
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
} // namespace JuicyEngine