#pragma once

#include "vulkan/vulkan.h"
#include <vector>
#include <JuicyEngine/Core/Window.h>

namespace JuicyEngine
{
void GetVulkanPlatformRequiredExtensionNames(std::vector<const char*>& names);
bool VulkanPlatformCreateSurface(VkInstance& instance, VkSurfaceKHR* surface, Window& window);
bool VulkanPlatformPresentationSupport(VkPhysicalDevice physicalDevice, unsigned int queueFamilyIndex);
}  // namespace JuicyEngine