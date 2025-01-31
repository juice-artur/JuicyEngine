#include "jepch.h"
#include "JuicyEngine/RHI/VulkanRHI/Platform/VulkanPlatform.h"
#include <vulkan/vulkan_win32.h>

#ifdef JE_PLATFORM_WINDOWS

namespace JuicyEngine
{
void GetVulkanPlatformRequiredExtensionNames(std::vector<const char*>& names)
{
    names.push_back("VK_KHR_win32_surface");
}
bool VulkanPlatformCreateSurface(VkInstance& instance, VkSurfaceKHR* surface, Window& window)
{
    VkWin32SurfaceCreateInfoKHR createInfo = {VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR};
    createInfo.hinstance = GetModuleHandle(nullptr);
    createInfo.hwnd = static_cast<HWND>(window.GetNativeWindow());

    VkResult result = vkCreateWin32SurfaceKHR(instance, &createInfo, nullptr, surface);
    if (result != VK_SUCCESS)
    {
        JE_CORE_FATAL("Vulkan surface creation failed.");
        return false;
    }

    return true;
}
bool VulkanPlatformPresentationSupport(VkPhysicalDevice physicalDevice, unsigned int queueFamilyIndex)
{
    return (bool)vkGetPhysicalDeviceWin32PresentationSupportKHR(physicalDevice, queueFamilyIndex);
}
}  // namespace JuicyEngine
#endif