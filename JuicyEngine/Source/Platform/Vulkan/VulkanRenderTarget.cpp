#include "jepch.h"
#include "VulkanRenderTarget.h"
#include "VulkanContext.h"
#include "VulkanUtils.h"
#include "Core/Core.h"

#include <imgui.h>
#include <backends/imgui_impl_vulkan.h>

namespace JuicyEngine
{

VulkanRenderTarget::VulkanRenderTarget(const CreateInfo& Info)
    : Width(Info.Width), Height(Info.Height), ColorFormat(Info.ColorFormat), DepthFormat(Info.DepthFormat),
    ImGuiLayout(Info.ImGuiLayout), ShadowSampler(Info.ShadowSampler),
    DepthFinalLayout(Info.DepthFinalLayout)
{
    CreateRenderPass();

    if (ColorFormat != VK_FORMAT_UNDEFINED)
    {
        ColorImage = std::make_unique<VulkanImage>(Width,
                                                   Height,
                                                   ColorFormat,
                                                   VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT |
                                                       Info.ExtraColorUsage);
        ColorImage->TransitionLayoutImmediate(VK_IMAGE_LAYOUT_UNDEFINED, Info.ImGuiLayout);
    }

    if (DepthFormat != VK_FORMAT_UNDEFINED)
    {
        VkImageUsageFlags DepthUsage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | Info.ExtraDepthUsage;
        if (ShadowSampler)
            DepthUsage |= VK_IMAGE_USAGE_SAMPLED_BIT;

        DepthImage = std::make_unique<VulkanImage>(Width, Height, DepthFormat, DepthUsage);
    }

    CreateSampler();

    std::vector<VkImageView> Attachments;
    if (ColorImage)
        Attachments.push_back(ColorImage->GetImageView());
    if (DepthImage)
        Attachments.push_back(DepthImage->GetImageView());

    Framebuffer = std::make_unique<VulkanFramebuffer>(RenderPass, VkExtent2D {Width, Height}, Attachments);

    if (Info.CreateImGuiDescriptor)
    {
        HasImGuiDescriptor = true;
        CreateDescriptor();
    }
}

VulkanRenderTarget::~VulkanRenderTarget()
{
    Destroy();
}

VulkanRenderTarget::VulkanRenderTarget(VulkanRenderTarget&& Other) noexcept
    : Width(Other.Width), Height(Other.Height), ColorFormat(Other.ColorFormat), DepthFormat(Other.DepthFormat),
      ImGuiLayout(Other.ImGuiLayout), ShadowSampler(Other.ShadowSampler), RenderPass(Other.RenderPass),
      ColorImage(std::move(Other.ColorImage)), DepthImage(std::move(Other.DepthImage)),
      Framebuffer(std::move(Other.Framebuffer)), Sampler(Other.Sampler), DescriptorSet(Other.DescriptorSet),
      HasImGuiDescriptor(Other.HasImGuiDescriptor),
      DepthFinalLayout(Other.DepthFinalLayout)
{
    Other.RenderPass = VK_NULL_HANDLE;
    Other.Sampler = VK_NULL_HANDLE;
    Other.DescriptorSet = VK_NULL_HANDLE;
    Other.HasImGuiDescriptor = false;
}

VulkanRenderTarget& VulkanRenderTarget::operator=(VulkanRenderTarget&& Other) noexcept
{
    if (this != &Other)
    {
        Destroy();

        Width = Other.Width;
        Height = Other.Height;
        ColorFormat = Other.ColorFormat;
        DepthFormat = Other.DepthFormat;
        ImGuiLayout = Other.ImGuiLayout;
        ShadowSampler = Other.ShadowSampler;
        RenderPass = Other.RenderPass;
        ColorImage = std::move(Other.ColorImage);
        DepthImage = std::move(Other.DepthImage);
        Framebuffer = std::move(Other.Framebuffer);
        Sampler = Other.Sampler;
        DescriptorSet = Other.DescriptorSet;
        HasImGuiDescriptor = Other.HasImGuiDescriptor;
        DepthFinalLayout = Other.DepthFinalLayout;

        Other.RenderPass = VK_NULL_HANDLE;
        Other.Sampler = VK_NULL_HANDLE;
        Other.DescriptorSet = VK_NULL_HANDLE;
        Other.HasImGuiDescriptor = false;
    }
    return *this;
}

void VulkanRenderTarget::Resize(uint32_t InWidth, uint32_t InHeight)
{
    if (InHeight == 0 || InWidth == 0)
        return;
    if (Width == InWidth && Height == InHeight)
        return;

    auto* Context = dynamic_cast<VulkanContext*>(VulkanContext::Get());
    vkDeviceWaitIdle(Context->GetDevice()->GetLogicalDevice());

    Destroy();
    Width = InWidth;
    Height = InHeight;

    CreateRenderPass();

    if (ColorFormat != VK_FORMAT_UNDEFINED)
    {
        ColorImage = std::make_unique<VulkanImage>(
            Width, Height, ColorFormat, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);
        ColorImage->TransitionLayoutImmediate(VK_IMAGE_LAYOUT_UNDEFINED, ImGuiLayout);
    }

    if (DepthFormat != VK_FORMAT_UNDEFINED)
    {
        VkImageUsageFlags DepthUsage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        if (ShadowSampler)
            DepthUsage |= VK_IMAGE_USAGE_SAMPLED_BIT;

        DepthImage = std::make_unique<VulkanImage>(Width, Height, DepthFormat, DepthUsage);
    }

    CreateSampler();

    std::vector<VkImageView> Attachments;
    if (ColorImage)
        Attachments.push_back(ColorImage->GetImageView());
    if (DepthImage)
        Attachments.push_back(DepthImage->GetImageView());

    Framebuffer = std::make_unique<VulkanFramebuffer>(RenderPass, VkExtent2D {Width, Height}, Attachments);

    if (HasImGuiDescriptor)
        CreateDescriptor();
}

VkFramebuffer VulkanRenderTarget::GetFramebuffer() const
{
    return Framebuffer ? Framebuffer->GetFramebuffer() : VK_NULL_HANDLE;
}

VkImageView VulkanRenderTarget::GetColorImageView() const
{
    return ColorImage ? ColorImage->GetImageView() : VK_NULL_HANDLE;
}

VkImageView VulkanRenderTarget::GetDepthImageView() const
{
    return DepthImage ? DepthImage->GetImageView() : VK_NULL_HANDLE;
}

void VulkanRenderTarget::CreateRenderPass()
{
    const auto* Context = dynamic_cast<VulkanContext*>(VulkanContext::Get());
    auto Device = Context->GetDevice()->GetLogicalDevice();

    std::vector<VkAttachmentDescription> Attachments;
    std::vector<VkAttachmentReference> ColorRefs;
    VkAttachmentReference DepthRef {};

    if (ColorFormat != VK_FORMAT_UNDEFINED)
    {
        VkAttachmentDescription ColorAttachment {};
        ColorAttachment.format = ColorFormat;
        ColorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        ColorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        ColorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        ColorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        ColorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        ColorAttachment.initialLayout = ImGuiLayout;
        ColorAttachment.finalLayout = ImGuiLayout;
        Attachments.push_back(ColorAttachment);

        VkAttachmentReference ColorRef {};
        ColorRef.attachment = 0;
        ColorRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        ColorRefs.push_back(ColorRef);
    }

    if (DepthFormat != VK_FORMAT_UNDEFINED)
    {
        VkAttachmentDescription DepthAttachment {};
        DepthAttachment.format = DepthFormat;
        DepthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        DepthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        DepthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        DepthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        DepthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        DepthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        DepthAttachment.finalLayout = DepthFinalLayout;
        Attachments.push_back(DepthAttachment);

        DepthRef.attachment = static_cast<uint32_t>(ColorRefs.size());
        DepthRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    }

    VkSubpassDescription Subpass {};
    Subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    Subpass.colorAttachmentCount = static_cast<uint32_t>(ColorRefs.size());
    Subpass.pColorAttachments = ColorRefs.data();
    if (DepthFormat != VK_FORMAT_UNDEFINED)
        Subpass.pDepthStencilAttachment = &DepthRef;

    VkSubpassDependency Dependency {};
    Dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    Dependency.dstSubpass = 0;
    Dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    Dependency.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    Dependency.dstStageMask =
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    Dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo RenderPassInfo {};
    RenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    RenderPassInfo.attachmentCount = static_cast<uint32_t>(Attachments.size());
    RenderPassInfo.pAttachments = Attachments.data();
    RenderPassInfo.subpassCount = 1;
    RenderPassInfo.pSubpasses = &Subpass;
    RenderPassInfo.dependencyCount = 1;
    RenderPassInfo.pDependencies = &Dependency;

    auto Result = vkCreateRenderPass(Device, &RenderPassInfo, nullptr, &RenderPass);
    JE_CORE_ASSERT(Result == VK_SUCCESS, "Failed to create render target render pass!");
}

void VulkanRenderTarget::CreateSampler()
{
    const auto* Context = dynamic_cast<VulkanContext*>(VulkanContext::Get());
    auto Device = Context->GetDevice()->GetLogicalDevice();

    VkSamplerCreateInfo SamplerInfo {};
    SamplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    SamplerInfo.magFilter = VK_FILTER_LINEAR;
    SamplerInfo.minFilter = VK_FILTER_LINEAR;
    SamplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    SamplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    SamplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    SamplerInfo.anisotropyEnable = VK_FALSE;
    SamplerInfo.maxAnisotropy = 1.0f;
    SamplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
    SamplerInfo.unnormalizedCoordinates = VK_FALSE;
    SamplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;

    if (ShadowSampler)
    {
        SamplerInfo.compareEnable = VK_TRUE;
        SamplerInfo.compareOp = VK_COMPARE_OP_LESS;
    }

    auto Result = vkCreateSampler(Device, &SamplerInfo, nullptr, &Sampler);
    JE_CORE_ASSERT(Result == VK_SUCCESS, "Failed to create render target sampler!");
}

void VulkanRenderTarget::CreateDescriptor()
{
    if (!ColorImage)
        return;

    DescriptorSet = ImGui_ImplVulkan_AddTexture(Sampler, ColorImage->GetImageView(), ImGuiLayout);
    JE_CORE_ASSERT(DescriptorSet != VK_NULL_HANDLE, "Failed to create render target ImGui descriptor!");
}

void VulkanRenderTarget::Destroy()
{
    if (HasImGuiDescriptor && DescriptorSet != VK_NULL_HANDLE)
    {
        if (ImGui::GetCurrentContext() != nullptr)
            ImGui_ImplVulkan_RemoveTexture(DescriptorSet);
        DescriptorSet = VK_NULL_HANDLE;
        HasImGuiDescriptor = false;
    }

    Framebuffer.reset();
    ColorImage.reset();
    DepthImage.reset();

    const auto* Context = dynamic_cast<VulkanContext*>(VulkanContext::Get());

    if (Sampler != VK_NULL_HANDLE)
    {
        vkDestroySampler(Context->GetDevice()->GetLogicalDevice(), Sampler, nullptr);
        Sampler = VK_NULL_HANDLE;
    }

    if (RenderPass != VK_NULL_HANDLE)
    {
        vkDestroyRenderPass(Context->GetDevice()->GetLogicalDevice(), RenderPass, nullptr);
        RenderPass = VK_NULL_HANDLE;
    }
}

} // namespace JuicyEngine
