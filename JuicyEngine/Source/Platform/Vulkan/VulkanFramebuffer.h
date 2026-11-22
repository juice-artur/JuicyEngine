#pragma once

#include <vulkan/vulkan.h>
#include <vector>

namespace JuicyEngine
{
class VulkanContext;

class VulkanFramebuffer
{
public:
    VulkanFramebuffer(VulkanContext& Context,
                      VkRenderPass RenderPass,
                      VkExtent2D Extent,
                      const std::vector<VkImageView>& Attachments);
    ~VulkanFramebuffer();

    VulkanFramebuffer(VulkanFramebuffer&& Other) noexcept;
    VulkanFramebuffer& operator=(VulkanFramebuffer&& Other) noexcept;

    VulkanFramebuffer(const VulkanFramebuffer&) = delete;
    VulkanFramebuffer& operator=(const VulkanFramebuffer&) = delete;

    VkFramebuffer GetFramebuffer() const
    {
        return Framebuffer;
    }
    VkExtent2D GetExtent() const
    {
        return Extent;
    }

private:
    VulkanContext& m_Context;
    VkFramebuffer Framebuffer = VK_NULL_HANDLE;
    VkExtent2D Extent {};
};

} // namespace JuicyEngine
