#pragma once

#include <vulkan/vulkan.h>

namespace JuicyEngine
{
	class VulkanSurface
	{
	public:
		VulkanSurface(const VkInstance& Instance);

		void Init(void* Window);
		void Shutdown();
		const VkSurfaceKHR& GetSurface();

	private:
		VkSurfaceKHR Surface;

		const VkInstance& Instance;
	};
} // namespace JuicyEngine