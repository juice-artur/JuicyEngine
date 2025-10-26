#include "VulkanShader.h"

#include <fstream>

#include "jepch.h"
#include "Core/Core.h"

std::vector<char> JuicyEngine::VulkanShader::ReadFile(const std::string& Filename)
{
    std::ifstream File(Filename, std::ios::ate | std::ios::binary);

    JE_CORE_ASSERT(File.is_open(), "Failed to open file!");

    size_t FileSize = File.tellg();
    std::vector<char> Buffer(FileSize);

    File.seekg(0);
    File.read(Buffer.data(), FileSize);

    File.close();

    return Buffer;
}

VkShaderModule JuicyEngine::VulkanShader::CreateShaderModule(VkDevice Device, const std::vector<char>& Code)
{
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = Code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(Code.data());

    VkShaderModule ShaderModule;
    auto Result = vkCreateShaderModule(Device, &createInfo, nullptr, &ShaderModule);

    JE_CORE_ASSERT(Result == VK_SUCCESS, "Failed to create shader module!");

    return ShaderModule;
}