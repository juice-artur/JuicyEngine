#pragma once

#include <vulkan/vulkan.h>

namespace JuicyEngine
{
	class VulkanRenderPass
	{
	public:
		void CreateRenderPass(const VkDevice Device,
		                      VkFormat SwapChainImageFormat,
		                      std::vector<VkImageView>& SwapChainImageViews,
		                      VkExtent2D SwapChainExtent);
		void Shutdown(const VkDevice Device);
		VkRenderPass GetVulkanRenderPass() const;
		void Begin(VkCommandBuffer CommandBuffer, VkExtent2D SwapChainExtent);
		void End(VkCommandBuffer CommandBuffer);
		uint32_t SwapChainImageIndex = 0;

	private:
		VkRenderPass RenderPass = VK_NULL_HANDLE;
		std::vector<VkFramebuffer> SwapChainFramebuffers;
	};
} // namespace JuicyEngine