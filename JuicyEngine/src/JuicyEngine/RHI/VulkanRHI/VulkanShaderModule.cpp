#include "jepch.h"

#include "JuicyEngine/RHI/VulkanRHI/VulkanShaderModule.h"
#include <JuicyEngine/Core/Core.h>

namespace JuicyEngine
{
VulkanShaderModule::VulkanShaderModule(VulkanDevice& device, const std::string& filename) : m_Device(device)
{
    std::vector<char> code = ReadFile(filename);

    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    VkResult result = vkCreateShaderModule(device.GetLogicalDevice(), &createInfo, nullptr, &m_ShaderModule);

    JE_CORE_ASSERT(result == VK_SUCCESS, "Failde create shader module");
}

VulkanShaderModule::~VulkanShaderModule() 
{
    vkDestroyShaderModule(m_Device.GetLogicalDevice(), m_ShaderModule, nullptr);
}

VkPipelineShaderStageCreateInfo VulkanShaderModule::CreateShaderStage(VkShaderStageFlagBits stage) const
{
    VkPipelineShaderStageCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    createInfo.stage = stage;
    createInfo.module = m_ShaderModule;
    createInfo.pName = "main";

    return createInfo;
}

std::vector<char> VulkanShaderModule::ReadFile(const std::string& filename)
{
    std::ifstream file(filename, std::ios::ate | std::ios::binary);
    if (!file.is_open())
    {
        JE_CORE_ASSERT(file.is_open(), "Failed to open file {0}", filename)
    }

    const auto fileSize = static_cast<size_t>(file.tellg());
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();

    return buffer;
}
}  // namespace JuicyEngine