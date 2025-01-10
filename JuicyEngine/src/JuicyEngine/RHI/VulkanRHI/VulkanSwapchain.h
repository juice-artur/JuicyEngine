#pragma once

#include "VulkanDevice.h"
#include <JuicyEngine/Core/Window.h>

namespace JuicyEngine
{
class VulkanSwapchain
{
public:
    VulkanSwapchain() = default;
    VulkanSwapchain(VulkanDevice& device, Window& windowHandle, VkSurfaceKHR& surface);
    void Create();
    void Destroy();

public:
    VkExtent2D swapChainExtent;
    VkFormat swapChainImageFormat;
    std::vector<VkImageView> swapChainImageViews;

private:
    VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
    void CreateSwapchainImageViews();

private:
    VulkanDevice& m_Device;
    Window& m_WindowHandle;
    VkSurfaceKHR* m_Surface = VK_NULL_HANDLE;
    VkSwapchainKHR m_SwapChain = VK_NULL_HANDLE;

    std::vector<VkImage> swapChainImages;
};
}  // namespace JuicyEngine