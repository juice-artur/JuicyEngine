#pragma once

#include "VulkanImage.h"
#include "VulkanFramebuffer.h"
#include <vulkan/vulkan.h>
#include <memory>

namespace JuicyEngine
{

class VulkanRenderTarget
{
public:
    struct CreateInfo
    {
        uint32_t Width = 0;
        uint32_t Height = 0;
        VkFormat ColorFormat = VK_FORMAT_B8G8R8A8_SRGB;
        VkFormat DepthFormat = VK_FORMAT_UNDEFINED;
        VkImageUsageFlags ExtraColorUsage = 0;
        VkImageUsageFlags ExtraDepthUsage = 0;
        bool CreateImGuiDescriptor = false;
        VkImageLayout ImGuiLayout = VK_IMAGE_LAYOUT_GENERAL;
        bool ShadowSampler = false;
        VkImageLayout DepthFinalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    };

    VulkanRenderTarget(const CreateInfo& Info);
    ~VulkanRenderTarget();

    VulkanRenderTarget(VulkanRenderTarget&& Other) noexcept;
    VulkanRenderTarget& operator=(VulkanRenderTarget&& Other) noexcept;

    VulkanRenderTarget(const VulkanRenderTarget&) = delete;
    VulkanRenderTarget& operator=(const VulkanRenderTarget&) = delete;

    void Resize(uint32_t Width, uint32_t Height);

    VkRenderPass GetRenderPass() const
    {
        return RenderPass;
    }
    VkFramebuffer GetFramebuffer() const;
    VkExtent2D GetExtent() const
    {
        return {Width, Height};
    }
    VkImageView GetColorImageView() const;
    VkImageView GetDepthImageView() const;
    VkSampler GetSampler() const
    {
        return Sampler;
    }
    VkDescriptorSet GetDescriptorSet() const
    {
        return DescriptorSet;
    }

private:
    void CreateRenderPass();
    void CreateSampler();
    void CreateDescriptor();
    void Destroy();

    uint32_t Width = 0;
    uint32_t Height = 0;
    VkFormat ColorFormat = VK_FORMAT_UNDEFINED;
    VkFormat DepthFormat = VK_FORMAT_UNDEFINED;
    VkImageLayout ImGuiLayout = VK_IMAGE_LAYOUT_GENERAL;
    bool ShadowSampler = false;
    
    VkImageLayout DepthFinalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkRenderPass RenderPass = VK_NULL_HANDLE;
    std::unique_ptr<VulkanImage> ColorImage;
    std::unique_ptr<VulkanImage> DepthImage;
    std::unique_ptr<VulkanFramebuffer> Framebuffer;
    VkSampler Sampler = VK_NULL_HANDLE;
    VkDescriptorSet DescriptorSet = VK_NULL_HANDLE;
    bool HasImGuiDescriptor = false;
};

} // namespace JuicyEngine
