#pragma once

#include "vulkan/vulkan.h"
#include "VulkanTypes.h"

namespace JuicyEngine
{

struct PhysicalDeviseRequirements
{
    bool Discrete;
    bool Graphics;
    bool Present;
    bool Transfer;
    std::vector<const char*> RequiredExtensions;
};

struct QueueFamilyIndices
{
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete() { return graphicsFamily.has_value() && presentFamily.has_value(); }
};

class VulkanDevice
{
public:
    VulkanDevice() = default;
    VulkanDevice(VkInstance* instance, VkSurfaceKHR& surface);
    void Init();
    void Cleanup();

    VkDevice& GetLogicalDevice();

    SwapChainSupportDetails QuerySwapChainSupport();
    QueueFamilyIndices FindQueueFamilies();
    // Queues descriptors
    VkQueue m_GraphicsQueue;
    VkQueue m_PresentQueue;

private:
    bool SelectPhysicalDevice();
    bool IsDeviceSuitable(VkPhysicalDevice device);
    QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
    bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
    SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);

private:
    VkInstance* m_Instance;
    VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
    VkDevice m_LogicalDevice = VK_NULL_HANDLE;
    VkSurfaceKHR m_Surface;

    PhysicalDeviseRequirements requirements = {
        .Discrete = true, .Graphics = true, .Present = true, .Transfer = true, .RequiredExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME}};

    //
};
}  // namespace JuicyEngine