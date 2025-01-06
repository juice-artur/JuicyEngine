#include "jepch.h"
#include "VulkanContext.h"
#include "Platform/VulkanPlatform.h"

namespace JuicyEngine
{
JuicyEngine::VulkanContext::VulkanContext(Window* windowHandle) : m_Window(windowHandle)
{
    JE_CORE_ASSERT(windowHandle, "Window handle is null!");
}
void VulkanContext::Init() 
{
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    // TODO: Change this to the actual name of the application
    appInfo.pApplicationName = "JuicyEngine";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "JuicyEngine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    std::vector<const char*> extensions;
    GetVulkanPlatformRequiredExtensionNames(extensions);

    // TODO: Add validation layers

    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    VkResult result = vkCreateInstance(&createInfo, nullptr, &m_Instance);


}
void VulkanContext::SwapBuffers()
{
    JE_CORE_ASSERT(true, "Implement this!");
}
}  // namespace JuicyEngine