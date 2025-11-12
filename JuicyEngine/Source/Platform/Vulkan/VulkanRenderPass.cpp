#include "VulkanRenderPass.h"

#include "VulkanContext.h"
#include "Core/Core.h"

#include <array>

namespace JuicyEngine
{

	void VulkanRenderPass::CreateRenderPass(VkFormat SwapChainImageFormat,
	                                        std::vector<VkImageView>& SwapChainImageViews,
	                                        VkExtent2D SwapChainExtent)
	{
		auto* Context = dynamic_cast<VulkanContext*>(VulkanContext::Get());
		
		VkAttachmentDescription ColorAttachment {};
		ColorAttachment.format = SwapChainImageFormat;
		ColorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		ColorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		ColorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		ColorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		ColorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		ColorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		ColorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		
		VkAttachmentReference ColorAttachmentRef =
		{
			.attachment = 0,
			.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
		};
		
		VkAttachmentDescription DepthAttachment{};
		DepthAttachment.format = Context->FindDepthFormat();
		DepthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		DepthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		DepthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		DepthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		DepthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		DepthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		DepthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkAttachmentReference DepthAttachmentRef =
		{
			.attachment = 1,
			.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
		};

		VkSubpassDescription Subpass {};
		Subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		Subpass.colorAttachmentCount = 1;
		Subpass.pColorAttachments = &ColorAttachmentRef;
		Subpass.pDepthStencilAttachment = &DepthAttachmentRef;

		VkSubpassDependency Dependency {};
		Dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		Dependency.dstSubpass = 0;
		Dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
		Dependency.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		Dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		Dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		std::array<VkAttachmentDescription, 2> Attachments = {ColorAttachment, DepthAttachment};
		
		VkRenderPassCreateInfo RenderPassInfo {};
		RenderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		RenderPassInfo.attachmentCount = static_cast<uint32_t>(Attachments.size());
		RenderPassInfo.pAttachments = Attachments.data();
		RenderPassInfo.subpassCount = 1;
		RenderPassInfo.pSubpasses = &Subpass;
		RenderPassInfo.pDependencies = &Dependency;
		RenderPassInfo.dependencyCount = 1;

		auto Result
		    = vkCreateRenderPass(Context->GetDevice()->GetLogicalDevice(), &RenderPassInfo, nullptr, &RenderPass);
		JE_CORE_ASSERT(Result == VK_SUCCESS, "Failed to create render pass!")

		SwapChainFramebuffers.resize(SwapChainImageViews.size());

		for (size_t i = 0; i < SwapChainImageViews.size(); i++)
		{
			std::array<VkImageView, 2> attachments[] = { SwapChainImageViews[i], Context->GetDepthImageView() };

			VkFramebufferCreateInfo FramebufferInfo {};
			FramebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			FramebufferInfo.renderPass = RenderPass;
			FramebufferInfo.attachmentCount = static_cast<uint32_t>(attachments->size());
			FramebufferInfo.pAttachments = attachments->data();
			FramebufferInfo.width = SwapChainExtent.width;
			FramebufferInfo.height = SwapChainExtent.height;
			FramebufferInfo.layers = 1;

			auto FramebufferResult = vkCreateFramebuffer(
			    Context->GetDevice()->GetLogicalDevice(), &FramebufferInfo, nullptr, &SwapChainFramebuffers[i]);
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

		std::array<VkClearValue, 2> ClearValues{};
		ClearValues[0].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
		ClearValues[1].depthStencil = {1.0f, 0};

		RenderPassInfo.clearValueCount = static_cast<uint32_t>(ClearValues.size());
		RenderPassInfo.pClearValues = ClearValues.data();

		vkCmdBeginRenderPass(CommandBuffer, &RenderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
	}

	void VulkanRenderPass::End(VkCommandBuffer CommandBuffer)
	{
		vkCmdEndRenderPass(CommandBuffer);
	}
} // namespace JuicyEngine