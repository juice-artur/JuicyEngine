#pragma once

#include <vulkan/vulkan.h>

namespace JuicyEngine
{
class VulkanImage
{
public:
    VulkanImage(uint32_t width,
                uint32_t height,
                VkFormat format,
                VkImageUsageFlags usage,
                VkMemoryPropertyFlags props = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    ~VulkanImage();
    
    VulkanImage(const VulkanImage&) = delete;
    VulkanImage& operator=(const VulkanImage&) = delete;
    VulkanImage(VulkanImage&&) = delete;
    VulkanImage& operator=(VulkanImage&&) = delete;

    VkImage GetImage() const;
    VkImageView GetImageView() const;
    VkFormat GetFormat() const;
    VkExtent2D GetExtent() const;

    void TransitionLayout(VkCommandBuffer Cmd,
                          VkImageLayout OldLayout,
                          VkImageLayout NewLayout,
                          VkImageAspectFlags Aspect = VK_IMAGE_ASPECT_COLOR_BIT);

    void TransitionLayoutImmediate(VkImageLayout OldLayout,
                                   VkImageLayout NewLayout,
                                   VkImageAspectFlags Aspect = VK_IMAGE_ASPECT_COLOR_BIT);

private:
    static void CreateRawImage(VkDevice Device,
                               uint32_t Width,
                               uint32_t Height,
                               VkFormat Format,
                               VkImageTiling Tiling,
                               VkImageUsageFlags Usage,
                               VkMemoryPropertyFlags Properties,
                               VkImage& OutImage,
                               VkDeviceMemory& OutImageMemory);

    static VkImageView CreateRawImageView(VkImage Image, VkFormat Format, VkImageAspectFlags AspectFlags);

private:
    VkImage Image;
    VkDeviceMemory Memory;
    VkImageView ImageView;
    VkFormat Format;
    uint32_t Width;
    uint32_t Height;
};

}