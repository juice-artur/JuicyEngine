#pragma once

#include <vulkan/vulkan.h>
#include <vector>

namespace JuicyEngine
{
struct PipelineCreateInfo
{
    VkRenderPass RenderPass = VK_NULL_HANDLE;
    std::vector<VkPipelineShaderStageCreateInfo> VulkanShaderModules;
};
} // namespace JuicyEngine
