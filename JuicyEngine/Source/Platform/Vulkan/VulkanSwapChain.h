#pragma once

#include "jepch.h"
#include "vulkan/vulkan.h"

namespace JuicyEngine
{
struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR Capabilities;
    std::vector<VkSurfaceFormatKHR> Formats;
    std::vector<VkPresentModeKHR> PresentModes;
};

class VulkanSwapChain
{
public:
    VulkanSwapChain();
    void Shutdown(VkDevice Device);
    
    void Init(VkPhysicalDevice PhysicalDevice, VkDevice Device, VkSurfaceKHR Surface, void* Window);
    static SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice Device, VkSurfaceKHR Surface);
    VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& AvailableFormats);
    VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& AvailablePresentModes);
    VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& Capabilities, void* Window);
private:
    VkSwapchainKHR SwapChain;
    uint32_t ImageCount;
    std::vector<VkImage> SwapChainImages;
    VkFormat SwapChainImageFormat;
    VkExtent2D SwapChainExtent;
};
}

