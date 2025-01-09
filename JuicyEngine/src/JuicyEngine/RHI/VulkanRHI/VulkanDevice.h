#pragma once

#include "vulkan/vulkan.h"

namespace JuicyEngine
{
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
    VulkanDevice(VkInstance* instance);
    void Init();
    void Cleanup();

private:
    bool SelectPhysicalDevice();
    bool IsDeviceSuitable(VkPhysicalDevice device);
    QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);

private:
    VkInstance* m_Instance;
    VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
    VkDevice m_LogicalDevice = VK_NULL_HANDLE;
    VkSurfaceKHR* m_Surface = VK_NULL_HANDLE;

    // Queues descriptors
    VkQueue m_GraphicsQueue;
    VkQueue m_PresentQueue;
    //
};
}  // namespace JuicyEngine