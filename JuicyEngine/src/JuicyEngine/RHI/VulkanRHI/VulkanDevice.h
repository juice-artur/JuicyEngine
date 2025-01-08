#pragma once

#include "vulkan/vulkan.h"

namespace JuicyEngine
{
struct QueueFamilyIndices
{
    std::optional<uint32_t> graphicsFamily;

    bool isComplete() { return graphicsFamily.has_value(); }
};

class VulkanDevice
{
public:
    VulkanDevice() = default;
    VulkanDevice(VkInstance* instance);
    void Init();

private:
    bool SelectPhysicalDevice();
    bool IsDeviceSuitable(VkPhysicalDevice device);
    QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);

private:
    VkInstance* m_Instance;
    VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
};
}  // namespace JuicyEngine