#include "VulkanFramebuffer.h"
#include "VulkanContext.h"
#include "Core/Core.h"

namespace JuicyEngine
{

VulkanFramebuffer::VulkanFramebuffer(VkRenderPass RenderPass,
                                     VkExtent2D Extent,
                                     const std::vector<VkImageView>& Attachments)
    : Extent(Extent)
{
    auto* Context = dynamic_cast<VulkanContext*>(VulkanContext::Get());
    auto Device = Context->GetDevice()->GetLogicalDevice();

    VkFramebufferCreateInfo info {};
    info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    info.renderPass = RenderPass;
    info.attachmentCount = static_cast<uint32_t>(Attachments.size());
    info.pAttachments = Attachments.data();
    info.width = Extent.width;
    info.height = Extent.height;
    info.layers = 1;

    VkResult Result = vkCreateFramebuffer(Device, &info, nullptr, &Framebuffer);
    JE_CORE_ASSERT(Result == VK_SUCCESS, "Failed to create framebuffer!");
}

VulkanFramebuffer::~VulkanFramebuffer()
{
    if (Framebuffer != VK_NULL_HANDLE)
    {
        auto* Context = dynamic_cast<VulkanContext*>(VulkanContext::Get());
        vkDestroyFramebuffer(Context->GetDevice()->GetLogicalDevice(), Framebuffer, nullptr);
    }
}

VulkanFramebuffer::VulkanFramebuffer(VulkanFramebuffer&& Other) noexcept
    : Framebuffer(Other.Framebuffer), Extent(Other.Extent)
{
    Other.Framebuffer = VK_NULL_HANDLE;
}

VulkanFramebuffer& VulkanFramebuffer::operator=(VulkanFramebuffer&& Other) noexcept
{
    if (this != &Other)
    {
        if (Framebuffer != VK_NULL_HANDLE)
        {
            auto* Context = dynamic_cast<VulkanContext*>(VulkanContext::Get());
            vkDestroyFramebuffer(Context->GetDevice()->GetLogicalDevice(), Framebuffer, nullptr);
        }
        Framebuffer = Other.Framebuffer;
        Extent = Other.Extent;
        Other.Framebuffer = VK_NULL_HANDLE;
    }

    return *this;
}

} // namespace JuicyEngine