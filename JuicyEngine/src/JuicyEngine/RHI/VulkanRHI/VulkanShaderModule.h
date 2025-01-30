#pragma once
#include "VulkanDevice.h"

namespace JuicyEngine
{
class VulkanShaderModule
{
public:
    VulkanShaderModule(VulkanDevice& device, const std::string& filename);
    ~VulkanShaderModule();
    VkPipelineShaderStageCreateInfo CreateShaderStage(VkShaderStageFlagBits stage) const;

private:
    std::vector<char> ReadFile(const std::string& filename);

private:
    VkShaderModule m_ShaderModule;
    VulkanDevice& m_Device;
};
}  // namespace JuicyEngine