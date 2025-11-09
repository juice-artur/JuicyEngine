#include "VulkanRenderPass.h"

#include "VulkanContext.h"
#include "Core/Core.h"

namespace JuicyEngine
{

	void VulkanRenderPass::CreateRenderPass(VkFormat SwapChainImageFormat,
	                                        std::vector<VkImageView>& SwapChainImageViews,
	                                        VkExtent2D SwapChainExtent)
	{
		VkAttachmentDescription ColorAttachment {};
		ColorAttachment.format = SwapChainImageFormat;
		ColorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		ColorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		ColorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		ColorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		ColorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		ColorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		ColorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference ColorAttachmentRef {};
		ColorAttachmentRef.attachment = 0;
		ColorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription Subpass {};
		Subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		Subpass.colorAttachmentCount = 1;
		Subpass.pColorAttachments = &ColorAttachmentRef;

		VkSubpassDependency Dependency {};
		Dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		Dependency.dstSubpass = 0;
		Dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		Dependency.srcAccessMask = 0;
		Dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		Dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		VkRenderPassCreateInfo RenderPassInfo {};
		RenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		RenderPassInfo.attachmentCount = 1;
		RenderPassInfo.pAttachments = &ColorAttachment;
		RenderPassInfo.subpassCount = 1;
		RenderPassInfo.pSubpasses = &Subpass;
		RenderPassInfo.pDependencies = &Dependency;
		RenderPassInfo.dependencyCount = 1;
		const auto* Context = dynamic_cast<VulkanContext*>(VulkanContext::Get());
		
		auto Result = vkCreateRenderPass(Context->GetDevice()->GetLogicalDevice(), &RenderPassInfo, nullptr, &RenderPass);
		JE_CORE_ASSERT(Result == VK_SUCCESS, "Failed to create render pass!")

		SwapChainFramebuffers.resize(SwapChainImageViews.size());

		for (size_t i = 0; i < SwapChainImageViews.size(); i++)
		{
			VkImageView attachments[] = { SwapChainImageViews[i] };

			VkFramebufferCreateInfo FramebufferInfo {};
			FramebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			FramebufferInfo.renderPass = RenderPass;
			FramebufferInfo.attachmentCount = 1;
			FramebufferInfo.pAttachments = attachments;
			FramebufferInfo.width = SwapChainExtent.width;
			FramebufferInfo.height = SwapChainExtent.height;
			FramebufferInfo.layers = 1;

			auto FramebufferResult = vkCreateFramebuffer(Context->GetDevice()->GetLogicalDevice(), &FramebufferInfo, nullptr, &SwapChainFramebuffers[i]);
			JE_CORE_ASSERT(FramebufferResult == VK_SUCCESS, "Failed to create framebuffer!")
		}
	}

	void VulkanRenderPass::Shutdown()
	{
		const auto* Context = dynamic_cast<VulkanContext*>(VulkanContext::Get());
		for (auto Framebuffer : SwapChainFramebuffers)
		{
			vkDestroyFramebuffer(Context->GetDevice()->GetLogicalDevice(), Framebuffer, nullptr);
		}
		vkDestroyRenderPass(Context->GetDevice()->GetLogicalDevice(), RenderPass, nullptr);
	}

	VkRenderPass VulkanRenderPass::GetVulkanRenderPass() const
	{
		return RenderPass;
	}

	void VulkanRenderPass::Begin(VkCommandBuffer CommandBuffer, VkExtent2D SwapChainExtent)
	{
		VkRenderPassBeginInfo RenderPassInfo {};
		RenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		RenderPassInfo.renderPass = RenderPass;
		RenderPassInfo.framebuffer = SwapChainFramebuffers[SwapChainImageIndex];
		RenderPassInfo.renderArea.offset = { 0, 0 };
		RenderPassInfo.renderArea.extent = SwapChainExtent;

		VkClearValue ClearColor = { { { 0.0f, 0.0f, 0.0f, 1.0f } } };
		RenderPassInfo.clearValueCount = 1;
		RenderPassInfo.pClearValues = &ClearColor;

		vkCmdBeginRenderPass(CommandBuffer, &RenderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
	}

	void VulkanRenderPass::End(VkCommandBuffer CommandBuffer)
	{
		vkCmdEndRenderPass(CommandBuffer);
	}
} // namespace JuicyEngine