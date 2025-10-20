# pragma once

#include <vulkan/vulkan.h>

struct QueueFamilyIndices
{
    std::optional<uint32_t> GraphicsFamily;

    bool IsComplete()
    {
        return GraphicsFamily.has_value();
    }
};

class VulkanDevice
{
public:
    VulkanDevice(const VkInstance& Instance);
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
};