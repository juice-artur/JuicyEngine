#include "jepch.h"

#include "VulkanDevice.h"
#include <JuicyEngine/Core/Core.h>
#include "Platform/VulkanPlatform.h"

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

VulkanDevice::VulkanDevice(VkInstance* instance)
{
    m_Instance = instance;
}

void JuicyEngine::VulkanDevice::Init()
{
    bool isFound = SelectPhysicalDevice();
    if (!isFound)
    {
        JE_CORE_ASSERT(false, "Couldn't found any  physical device");
    }

    QueueFamilyIndices indices = FindQueueFamilies(m_PhysicalDevice);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<unsigned int> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};

    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies)
    {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures{};

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.queueCreateInfoCount = queueCreateInfos.size();
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.pEnabledFeatures = &deviceFeatures;

    VkResult result = vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &m_LogicalDevice);

    JE_ASSERT(result == VK_SUCCESS, "Failed to create logical device!");

    vkGetDeviceQueue(m_LogicalDevice, indices.graphicsFamily.value(), 0, &m_GraphicsQueue);
    vkGetDeviceQueue(m_LogicalDevice, indices.presentFamily.value(), 0, &m_PresentQueue);
}

void VulkanDevice::Cleanup()
{
    vkDestroyDevice(m_LogicalDevice, nullptr);
}

bool VulkanDevice::SelectPhysicalDevice()
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(*m_Instance, &deviceCount, nullptr);

    JE_CORE_ASSERT(deviceCount != 0, "Couldn't found any  physical device");

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(*m_Instance, &deviceCount, devices.data());

    PhysicalDeviseRequirements requirements = {
        .Discrete = true, .Graphics = true, .Present = true, .Transfer = true, .RequiredExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME}};

    for (auto& curentPhysicalDevice : devices)
    {
        if (IsDeviceSuitable(curentPhysicalDevice))
        {
            m_PhysicalDevice = curentPhysicalDevice;
            return true;
        }
    }

    return false;
}
bool VulkanDevice::IsDeviceSuitable(VkPhysicalDevice device)
{
    QueueFamilyIndices indices = FindQueueFamilies(device);

    return indices.isComplete();
}
QueueFamilyIndices VulkanDevice::FindQueueFamilies(VkPhysicalDevice device)
{
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    for (const auto& queueFamily : queueFamilies)
    {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            indices.graphicsFamily = i;
        }

        if (VulkanPlatformPresentationSupport(device, i))
        {
            indices.presentFamily = i;
        }

        if (indices.isComplete())
        {
            break;
        }

        i++;
    }

    return indices;
}
}  // namespace JuicyEngine