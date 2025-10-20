#include "VulkanDevice.h"
#include "jepch.h"
#include "Core/Core.h"

VulkanDevice::VulkanDevice(const VkInstance& Instance) : PhysicalDevice(VK_NULL_HANDLE)
{
    PickPhysicalDevice(Instance);
    
    CreateLogicalDevice();

    vkGetDeviceQueue(LogicalDevice, QueueIndices.GraphicsFamily.value(), 0, &GraphicsQueue);
}

VulkanDevice::~VulkanDevice()
{
    vkDestroyDevice(LogicalDevice, nullptr);
}

void VulkanDevice::PickPhysicalDevice(const VkInstance& Instance)
{
    uint32_t DeviceCount = 0;
    vkEnumeratePhysicalDevices(Instance, &DeviceCount, nullptr);

    JE_CORE_ASSERT(DeviceCount, "Failed to find GPUs with Vulkan support!");
    
    std::vector<VkPhysicalDevice> Devices(DeviceCount);
    vkEnumeratePhysicalDevices(Instance, &DeviceCount, Devices.data());

    for (const auto& CurrentDevice : Devices)
    {
        if (IsDeviceSuitable(CurrentDevice)) {
            PhysicalDevice = CurrentDevice;
            break;
        }
    }
    
    JE_CORE_ASSERT(PhysicalDevice, "Failed to find a suitable GPU!");

    QueueIndices = FindQueueFamilies(PhysicalDevice);
    
    JE_INFO("Device meets queue requirements.");
    JE_TRACE("Graphics Family Index: {0}", QueueIndices.GraphicsFamily.value());
}

bool VulkanDevice::IsDeviceSuitable(VkPhysicalDevice Device)
{
    QueueFamilyIndices Indices = FindQueueFamilies(Device);

    return Indices.IsComplete();
}

void VulkanDevice::CreateLogicalDevice()
{
    VkDeviceQueueCreateInfo QueueCreateInfo{};
    QueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    QueueCreateInfo.queueFamilyIndex = QueueIndices.GraphicsFamily.value();
    QueueCreateInfo.queueCount = 1;
    float QueuePriority = 1.0f;
    QueueCreateInfo.pQueuePriorities = &QueuePriority;

    VkPhysicalDeviceFeatures DeviceFeatures{};
    
    VkDeviceCreateInfo DeviceCreateInfo{};
    DeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    DeviceCreateInfo.queueCreateInfoCount = 1;
    DeviceCreateInfo.pQueueCreateInfos = &QueueCreateInfo;
    DeviceCreateInfo.pEnabledFeatures = &DeviceFeatures;
    
    VkResult Result = vkCreateDevice(PhysicalDevice, &DeviceCreateInfo, nullptr, &LogicalDevice);

    JE_ASSERT(Result == VK_SUCCESS, "Failed to create logical device!");
}

QueueFamilyIndices VulkanDevice::FindQueueFamilies(VkPhysicalDevice Device)
{
    QueueFamilyIndices Indices;
    
    uint32_t QueueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(Device, &QueueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> QueueFamilies(QueueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(Device, &QueueFamilyCount, QueueFamilies.data());

    int i = 0;
    for (const auto& queueFamily : QueueFamilies) {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            Indices.GraphicsFamily = i;
        }
        
        i++;
    }

    return Indices;
}