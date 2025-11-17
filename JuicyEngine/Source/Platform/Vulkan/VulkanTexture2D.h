#pragma once

#include "VulkanBuffer.h"
#include "vulkan/vulkan.h"

namespace JuicyEngine
{
	class VulkanTexture2D
	{
	public:
		VulkanTexture2D(const std::string& Path);
		~VulkanTexture2D();
		VkImageView& GetImageView();
		VkSampler& GetSampler();

		VulkanTexture2D(const VulkanTexture2D&) = delete;
		VulkanTexture2D& operator=(const VulkanTexture2D&) = delete;
		VulkanTexture2D(VulkanTexture2D&&) = delete;
		VulkanTexture2D& operator=(VulkanTexture2D&&) = delete;
	
	public:
		static void CreateImage(uint32_t Width, uint32_t Height, VkFormat Format, VkImageTiling Tiling, VkImageUsageFlags Usage,
			VkMemoryPropertyFlags Properties, VkImage& Image, VkDeviceMemory& ImageMemory);

		static VkImageView CreateImageView(VkImage Image, VkFormat Format, VkImageAspectFlags AspectFlags);
		
	protected:
		void CreateImage(VkImageTiling Tiling,
			VkImageUsageFlags Usage, VkMemoryPropertyFlags Properties);

		void CreateTextureSampler();

		void TransitionImageLayout(VkImageLayout OldLayout, VkImageLayout NewLayout);
		void UploadData(VkBuffer Buffer);
		
	private:
		VkImage Image;
		VkFormat Format;
		VkDeviceMemory ImageMemory;
		int32_t Width;
		int32_t Height;

		VkImageView ImageView;
		VkSampler TextureSampler;
	};
}