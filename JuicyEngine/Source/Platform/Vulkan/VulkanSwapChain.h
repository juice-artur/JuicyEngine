#pragma once

#include "jepch.h"
#include "vulkan/vulkan.h"

namespace JuicyEngine
{
class VulkanContext;

struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR Capabilities;
    std::vector<VkSurfaceFormatKHR> Formats;
    std::vector<VkPresentModeKHR> PresentModes;
};

class VulkanSwapChain
{
public:
    explicit VulkanSwapChain(VulkanContext& Context) : m_Context(Context) {}
    void Shutdown() const;

    void Init(VkSurfaceKHR Surface, void* Window);
    static SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice Device, VkSurfaceKHR Surface);
    VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& AvailableFormats);
    VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& AvailablePresentModes);
    VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& Capabilities, void* Window);
    VkFormat GetFormat() const;
    VkExtent2D GetExtent() const;
    std::vector<VkImageView>& GetSwapChainImageViews();
    const std::vector<VkImage>& GetSwapChainImages() const;
    VkSwapchainKHR GetSwapChain() const;
    uint32_t GetImageCount() const;

private:
    void CreateImageViews(const VkDevice Device);

private:
    VulkanContext& m_Context;
    VkSwapchainKHR SwapChain;
    uint32_t ImageCount;
    std::vector<VkImage> SwapChainImages;
    VkFormat SwapChainImageFormat;
    VkExtent2D SwapChainExtent;
    std::vector<VkImageView> SwapChainImageViews;
};
} // namespace JuicyEngine
