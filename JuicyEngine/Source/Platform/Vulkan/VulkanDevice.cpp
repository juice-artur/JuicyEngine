#include "VulkanDevice.h"

#include <set>

#include "jepch.h"
#include "VulkanSwapChain.h"
#include "Core/Core.h"

namespace JuicyEngine
{
VulkanDevice::VulkanDevice(const VkInstance& Instance, const VkSurfaceKHR& Surface) :
    PhysicalDevice(VK_NULL_HANDLE),
    Surface(Surface)
{
    PickPhysicalDevice(Instance);
    
    QueueIndices = FindQueueFamilies(PhysicalDevice, Surface);
    JE_CORE_ASSERT(QueueIndices.IsComplete(), "Could not find Queue indices")
    CreateLogicalDevice();
    
    vkGetDeviceQueue(LogicalDevice, QueueIndices.GraphicsFamily.value(), 0, &GraphicsQueue);
    vkGetDeviceQueue(LogicalDevice, QueueIndices.PresentFamily.value(), 0, &PresentQueue);
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
}

bool VulkanDevice::IsDeviceSuitable(VkPhysicalDevice Device)
{
    QueueFamilyIndices Indices = FindQueueFamilies(Device, Surface);
    bool bExtensionsSupported = IsDeviceExtensionSupport(Device);

    bool bSwapChainAdequate = false;
    if (bExtensionsSupported) {
        SwapChainSupportDetails SwapChainSupport = VulkanSwapChain::QuerySwapChainSupport(Device, Surface);
        bSwapChainAdequate = !SwapChainSupport.Formats.empty() && !SwapChainSupport.PresentModes.empty();
    }

    return Indices.IsComplete() && bExtensionsSupported && bSwapChainAdequate;
}

bool VulkanDevice::IsDeviceExtensionSupport(VkPhysicalDevice Device)
{
    uint32_t ExtensionCount;
    vkEnumerateDeviceExtensionProperties(Device, nullptr, &ExtensionCount, nullptr);

    std::vector<VkExtensionProperties> AvailableExtensions(ExtensionCount);
    vkEnumerateDeviceExtensionProperties(Device, nullptr, &ExtensionCount, AvailableExtensions.data());

    std::set<std::string> RequiredExtensions(DeviceExtensions.begin(), DeviceExtensions.end());

    for (const auto& extension : AvailableExtensions) {
        RequiredExtensions.erase(extension.extensionName);
    }

    return RequiredExtensions.empty();
}

void VulkanDevice::CreateLogicalDevice()
{
    std::vector<VkDeviceQueueCreateInfo> QueueCreateInfos;
    std::set<uint32_t> UniqueQueueFamilies = {QueueIndices.GraphicsFamily.value(), QueueIndices.PresentFamily.value()};

    float QueuePriority = 1.0f;
    for (uint32_t QueueFamily : UniqueQueueFamilies) {
        VkDeviceQueueCreateInfo QueueCreateInfo{};
        QueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        QueueCreateInfo.queueFamilyIndex = QueueFamily;
        QueueCreateInfo.queueCount = 1;
        QueueCreateInfo.pQueuePriorities = &QueuePriority;
        QueueCreateInfos.push_back(QueueCreateInfo);
    }

    VkPhysicalDeviceFeatures DeviceFeatures{};
    
    VkDeviceCreateInfo DeviceCreateInfo{};
    DeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    DeviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(DeviceExtensions.size());
    DeviceCreateInfo.ppEnabledExtensionNames = DeviceExtensions.data();
    DeviceCreateInfo.queueCreateInfoCount = QueueCreateInfos.size();
    DeviceCreateInfo.pQueueCreateInfos = QueueCreateInfos.data();
    DeviceCreateInfo.pEnabledFeatures = &DeviceFeatures;
    
    VkResult Result = vkCreateDevice(PhysicalDevice, &DeviceCreateInfo, nullptr, &LogicalDevice);

    JE_ASSERT(Result == VK_SUCCESS, "Failed to create logical device!");
}

QueueFamilyIndices VulkanDevice::FindQueueFamilies(VkPhysicalDevice Device, VkSurfaceKHR Surface)
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

        VkBool32 bPresentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(Device, i, Surface, &bPresentSupport);

        if (bPresentSupport) {
            Indices.PresentFamily = i;
        }

        if (Indices.IsComplete()) {
            break;
        }

        i++;
    }

    return Indices;
}

VkPhysicalDevice VulkanDevice::GetPhysicalDevice() const
{
    return PhysicalDevice;
}

VkDevice VulkanDevice::GetLogicalDevice() const
{
    return LogicalDevice;
}
}