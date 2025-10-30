#pragma once

#include <vulkan/vulkan.h>

namespace JuicyEngine
{
struct PipelineCreateInfo;

class VulkanPipeline
{
public:
    void Create(const VkDevice Device, const PipelineCreateInfo& Info);

    void Shutdown(const VkDevice Device);

    void Bind(VkCommandBuffer CommandBuffer);

private:
    VkPipelineLayout PipelineLayout = VK_NULL_HANDLE;
    VkPipeline Pipeline = VK_NULL_HANDLE;
};
}