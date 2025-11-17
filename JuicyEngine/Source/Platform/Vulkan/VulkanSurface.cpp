#define VK_USE_PLATFORM_WIN32_KHR

#include "VulkanSurface.h"
#include <windows.h>
#include "Core/Core.h"
#include <vulkan/vulkan_win32.h>

namespace JuicyEngine
{
	VulkanSurface::VulkanSurface(const VkInstance& Instance) : Instance(Instance) {}

	void VulkanSurface::Init(void* Window)
	{
		JE_CORE_ASSERT(Window, "Window handle is null!")

		HINSTANCE hInstance = GetModuleHandle(nullptr);

		VkWin32SurfaceCreateInfoKHR SurfaceCreateInfo {};
		SurfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		SurfaceCreateInfo.hinstance = hInstance;
		SurfaceCreateInfo.hwnd = static_cast<HWND>(Window);

		VkResult result = vkCreateWin32SurfaceKHR(Instance, &SurfaceCreateInfo, nullptr, &Surface);
		JE_CORE_ASSERT(result == VK_SUCCESS, "Failed to create Win32 Vulkan surface!")
	}

	void VulkanSurface::Shutdown()
	{
		vkDestroySurfaceKHR(Instance, Surface, nullptr);
	}

	const VkSurfaceKHR& VulkanSurface::GetSurface()
	{
		return Surface;
	}
} // namespace JuicyEngine