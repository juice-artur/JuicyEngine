#pragma once

#include <vulkan/vulkan_core.h>
#include <vector>

namespace JuicyEngine
{
struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};
}  // namespace JuicyEngine
