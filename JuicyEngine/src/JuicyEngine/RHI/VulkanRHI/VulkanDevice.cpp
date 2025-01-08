#include "jepch.h"

#include "VulkanDevice.h"
#include <JuicyEngine/Core/Core.h>

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

        if (indices.isComplete())
        {
            break;
        }

        i++;
    }

    return indices;
}
}  // namespace JuicyEngine