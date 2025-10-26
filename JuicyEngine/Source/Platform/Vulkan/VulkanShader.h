#pragma once
#include <vulkan/vulkan.h>


namespace JuicyEngine
{
class VulkanShader
{
public:
    static std::vector<char> ReadFile(const std::string& Filename);
    static VkShaderModule CreateShaderModule(VkDevice Device, const std::vector<char>& Code);
    
private:
    
};
}