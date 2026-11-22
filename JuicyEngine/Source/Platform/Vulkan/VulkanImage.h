#pragma once

#include <vulkan/vulkan.h>

namespace JuicyEngine
{
class VulkanContext;

class VulkanImage
{
public:
    VulkanImage(VulkanContext& Context,
                uint32_t width,
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
    void CreateRawImage(uint32_t InWidth,
                        uint32_t InHeight,
                        VkFormat InFormat,
                        VkImageTiling Tiling,
                        VkImageUsageFlags Usage,
                        VkMemoryPropertyFlags Properties,
                        VkImage& OutImage,
                        VkDeviceMemory& OutImageMemory);

    VkImageView CreateRawImageView(VkImage InImage, VkFormat InFormat, VkImageAspectFlags AspectFlags);

private:
    VulkanContext& m_Context;
    VkImage Image;
    VkDeviceMemory Memory;
    VkImageView ImageView;
    VkFormat Format;
    uint32_t Width;
    uint32_t Height;
};

} // namespace JuicyEngine
