#pragma once

#include "VulkanRenderPass.h"

namespace JuicyEngine
{
	struct PipelineCreateInfo
	{
		std::shared_ptr<VulkanRenderPass> RenderPass;
		std::vector<VkPipelineShaderStageCreateInfo> VulkanShaderModules;
	};
} // namespace JuicyEngine