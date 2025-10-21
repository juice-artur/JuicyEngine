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

    static QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice Device, VkSurfaceKHR Surface);

    VkPhysicalDevice GetPhysicalDevice() const;
    VkDevice GetLogicalDevice() const;

private:
    void PickPhysicalDevice(const VkInstance& Instance);
    bool IsDeviceSuitable(VkPhysicalDevice Device);
    bool IsDeviceExtensionSupport(VkPhysicalDevice Device);
    void CreateLogicalDevice();

private:
    VkPhysicalDevice PhysicalDevice;
    VkDevice LogicalDevice;
    QueueFamilyIndices QueueIndices;
    VkQueue GraphicsQueue;
    VkQueue PresentQueue;
    const VkSurfaceKHR& Surface;

    const std::vector<const char*> DeviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };
};
}