#pragma once

#include "VulkanFramebuffer.h"

#include <vulkan/vulkan.h>
#include <vector>

namespace JuicyEngine
{
class VulkanContext;

class VulkanRenderPass
{
public:
    explicit VulkanRenderPass(VulkanContext& Context) : m_Context(Context) {}

    void CreateRenderPass(VkFormat SwapChainImageFormat,
                          std::vector<VkImageView>& SwapChainImageViews,
                          VkExtent2D SwapChainExtent);
    void Shutdown();
    void SetRenderPass(VkRenderPass InRenderPass);
    VkRenderPass GetVulkanRenderPass() const;
    VkFramebuffer GetFramebuffer(uint32_t Index) const;
    void Begin(VkCommandBuffer CommandBuffer, VkExtent2D SwapChainExtent);
    void End(VkCommandBuffer CommandBuffer);
    uint32_t SwapChainImageIndex = 0;

private:
    VulkanContext& m_Context;
    VkRenderPass RenderPass = VK_NULL_HANDLE;
    std::vector<std::unique_ptr<VulkanFramebuffer>> SwapChainFramebuffers;
};
} // namespace JuicyEngine
