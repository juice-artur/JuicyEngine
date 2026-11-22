#pragma once

#include <vulkan/vulkan.h>

namespace JuicyEngine
{
class VulkanContext;
struct PipelineCreateInfo;

class VulkanPipeline
{
public:
    explicit VulkanPipeline(VulkanContext& Context) : m_Context(Context) {}

    void Create(const PipelineCreateInfo& Info);

    void Shutdown();

    void Bind(VkCommandBuffer CommandBuffer);

    VkDescriptorSetLayout& GetDescriptorSetLayout();

    VkPipelineLayout& GetPipelineLayout();

private:
    VulkanContext& m_Context;
    VkPipelineLayout PipelineLayout = VK_NULL_HANDLE;
    VkPipeline Pipeline = VK_NULL_HANDLE;
    VkDescriptorSetLayout DescriptorSetLayout = VK_NULL_HANDLE;
};
} // namespace JuicyEngine
