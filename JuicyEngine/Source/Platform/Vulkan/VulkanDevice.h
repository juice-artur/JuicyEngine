#pragma once

#include <vulkan/vulkan.h>

namespace JuicyEngine
{
struct QueueFamilyIndices
{
    std::optional<uint32_t> GraphicsFamily;
    std::optional<uint32_t> PresentFamily;
    
    bool IsComplete() const
    {
        return GraphicsFamily.has_value() && PresentFamily.has_value();
    }
};

class VulkanDevice
{
public:
    VulkanDevice(const VkInstance& Instance, const VkSurfaceKHR& Surface);
    ~VulkanDevice();

private:
    void PickPhysicalDevice(const VkInstance& Instance);
    bool IsDeviceSuitable(VkPhysicalDevice Device);
    void CreateLogicalDevice();

    QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice Device);

private:
    VkPhysicalDevice PhysicalDevice;
    VkDevice LogicalDevice;
    QueueFamilyIndices QueueIndices;
    VkQueue GraphicsQueue;
    VkQueue PresentQueue;
    const VkSurfaceKHR& Surface;
};
}