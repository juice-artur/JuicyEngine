#include "VulkanSwapChain.h"
#define NOMINMAX
#include <windows.h>
#include <limits>

#include "VulkanDevice.h"
#include "Core/Core.h"

namespace JuicyEngine
{
VulkanSwapChain::VulkanSwapChain()
{
}

void VulkanSwapChain::Shutdown(VkDevice Device)
{
    for (auto ImageView : SwapChainImageViews) {
        vkDestroyImageView(Device, ImageView, nullptr);
    }
    vkDestroySwapchainKHR(Device, SwapChain, nullptr);
}

void VulkanSwapChain::Init(VkPhysicalDevice PhysicalDevice, VkDevice Device, VkSurfaceKHR Surface, void* Window)
{
    SwapChainSupportDetails SwapChainSupport = QuerySwapChainSupport(PhysicalDevice, Surface);

    VkSurfaceFormatKHR SurfaceFormat = ChooseSwapSurfaceFormat(SwapChainSupport.Formats);
    VkPresentModeKHR PresentMode = ChooseSwapPresentMode(SwapChainSupport.PresentModes);
    VkExtent2D Extent = ChooseSwapExtent(SwapChainSupport.Capabilities, Window);

    ImageCount = SwapChainSupport.Capabilities.minImageCount + 1;

    if (SwapChainSupport.Capabilities.maxImageCount > 0 && ImageCount > SwapChainSupport.Capabilities.maxImageCount)
    {
        ImageCount = SwapChainSupport.Capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR SwapchainCreateInfo{};
    SwapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    SwapchainCreateInfo.surface = Surface;

    SwapchainCreateInfo.minImageCount = ImageCount;
    SwapchainCreateInfo.imageFormat = SurfaceFormat.format;
    SwapchainCreateInfo.imageColorSpace = SurfaceFormat.colorSpace;
    SwapchainCreateInfo.imageExtent = Extent;
    SwapchainCreateInfo.imageArrayLayers = 1;
    SwapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices Indices = VulkanDevice::FindQueueFamilies(PhysicalDevice, Surface);
    uint32_t queueFamilyIndices[] = {Indices.GraphicsFamily.value(), Indices.GraphicsFamily.value()};

    if (Indices.GraphicsFamily != Indices.GraphicsFamily)
    {
        SwapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        SwapchainCreateInfo.queueFamilyIndexCount = 2;
        SwapchainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else
    {
        SwapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    SwapchainCreateInfo.preTransform = SwapChainSupport.Capabilities.currentTransform;
    SwapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    SwapchainCreateInfo.presentMode = PresentMode;
    SwapchainCreateInfo.clipped = VK_TRUE;

    SwapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

    VkResult Result = vkCreateSwapchainKHR(Device, &SwapchainCreateInfo, nullptr, &SwapChain);
    JE_CORE_ASSERT(Result == VK_SUCCESS, "Failed to create Swap Chain!");

    vkGetSwapchainImagesKHR(Device, SwapChain, &ImageCount, nullptr);
    SwapChainImages.resize(ImageCount);
    vkGetSwapchainImagesKHR(Device, SwapChain, &ImageCount, SwapChainImages.data());

    SwapChainImageFormat = SurfaceFormat.format;
    SwapChainExtent = Extent;

    CreateImageViews(Device);
}

SwapChainSupportDetails VulkanSwapChain::QuerySwapChainSupport(VkPhysicalDevice Device, VkSurfaceKHR Surface)
{
    SwapChainSupportDetails Details{};

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(Device, Surface, &Details.Capabilities);
    uint32_t FormatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(Device, Surface, &FormatCount, nullptr);

    if (FormatCount != 0)
    {
        Details.Formats.resize(FormatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(Device, Surface, &FormatCount, Details.Formats.data());
    }

    uint32_t PresentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(Device, Surface, &PresentModeCount, nullptr);

    if (PresentModeCount != 0)
    {
        Details.PresentModes.resize(PresentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(Device, Surface, &PresentModeCount, Details.PresentModes.data());
    }
    return Details;
}

VkSurfaceFormatKHR VulkanSwapChain::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& AvailableFormats)
{
    JE_CORE_ASSERT(!AvailableFormats.empty(), "No surface formats available");
    for (const auto& AvailableFormat : AvailableFormats)
    {
        if (AvailableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && AvailableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            return AvailableFormat;
        }
    }
    return AvailableFormats[0];
}

VkPresentModeKHR VulkanSwapChain::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& AvailablePresentModes)
{
    for (const auto& AvailablePresentMode : AvailablePresentModes)
    {
        if (AvailablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            return AvailablePresentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D VulkanSwapChain::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& Capabilities, void* Window)
{
    if (Capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
    {
        return Capabilities.currentExtent;
    }
    else
    {
        RECT Rect;
        GetClientRect(static_cast<HWND>(Window), &Rect);

        uint32_t Width = Rect.right - Rect.left;
        uint32_t Height = Rect.bottom - Rect.top;

        VkExtent2D ActualExtent =
        {
            std::clamp(Width, Capabilities.minImageExtent.width, Capabilities.maxImageExtent.width),
            std::clamp(Height, Capabilities.minImageExtent.height, Capabilities.maxImageExtent.height)
        };

        return ActualExtent;
    }
}

VkFormat VulkanSwapChain::GetFormat() const
{
    return SwapChainImageFormat;
}

VkExtent2D VulkanSwapChain::GetExtent() const
{
    return SwapChainExtent;
}

std::vector<VkImageView>& VulkanSwapChain::GetSwapChainImageViews()
{
    return SwapChainImageViews;
}

VkSwapchainKHR VulkanSwapChain::GetSwapChain() const
{
    return SwapChain;
}

void VulkanSwapChain::CreateImageViews(const VkDevice Device)
{
    SwapChainImageViews.resize(SwapChainImages.size());
    for (size_t i = 0; i < SwapChainImages.size(); i++)
    {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = SwapChainImages[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = SwapChainImageFormat;
        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        auto Result = vkCreateImageView(Device, &createInfo, nullptr, &SwapChainImageViews[i]);
        JE_CORE_ASSERT(Result == VK_SUCCESS, "Failed to create image views!");
    }
}
}
