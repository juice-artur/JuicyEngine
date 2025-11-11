#pragma once

#include <vulkan/vulkan.h>

namespace JuicyEngine
{
	struct PipelineCreateInfo;

	class VulkanPipeline
	{
	public:
		void Create(const PipelineCreateInfo& Info);

		void Shutdown();

		void Bind(VkCommandBuffer CommandBuffer);

		VkDescriptorSetLayout& GetDescriptorSetLayout();

		VkPipelineLayout& GetPipelineLayout();

	private:
		VkPipelineLayout PipelineLayout = VK_NULL_HANDLE;
		VkPipeline Pipeline = VK_NULL_HANDLE;
		VkDescriptorSetLayout DescriptorSetLayout = VK_NULL_HANDLE;
	};
} // namespace JuicyEngine