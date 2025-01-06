#include "jepch.h"
#include "VulkanContext.h"
#include "Platform/VulkanPlatform.h"

namespace JuicyEngine
{
JuicyEngine::VulkanContext::VulkanContext(Window* windowHandle) : m_Window(windowHandle)
{
    JE_CORE_ASSERT(windowHandle, "Window handle is null!");
}
void VulkanContext::Init()
{
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    // TODO: Change this to the actual name of the application
    appInfo.pApplicationName = "JuicyEngine";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "JuicyEngine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    std::vector<const char*> extensions;
    GetVulkanPlatformRequiredExtensionNames(extensions);

    // TODO: Add validation layers
#ifdef JE_DEBUG
    extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    unsigned int availableExtensionCount = 0;
    vkEnumerateInstanceExtensionProperties(0, &availableExtensionCount, 0);
    std::vector<VkExtensionProperties> availableExtensions;
    availableExtensions.resize(availableExtensionCount);
    vkEnumerateInstanceExtensionProperties(0, &availableExtensionCount, availableExtensions.data());

    // Verify required extensions are available.
    for (unsigned int i = 0; i < extensions.size(); ++i)
    {
        bool found = false;
        for (unsigned int j = 0; j < availableExtensionCount; ++j)
        {
            if (std::strcmp(extensions[i], availableExtensions[j].extensionName) == 0)
            {
                found = true;
                JE_INFO("Required exension found: {0}...", extensions[i]);
                break;
            }
        }

        if (!found)
        {
            JE_CORE_FATAL("Required extension is missing: {0}", extensions[i]);
        }
    }

    const std::vector<const char*> requiredValidationLayers = {"VK_LAYER_KHRONOS_validation"};
    unsigned int requiredValidationLayerCount = requiredValidationLayers.size();

    // Obtain a list of available validation layers
    unsigned int availableLayerCount = 0;

    vkEnumerateInstanceLayerProperties(&availableLayerCount, 0);
    std::vector<VkLayerProperties> availableLayers;
    availableLayers.resize(availableLayerCount);
    vkEnumerateInstanceLayerProperties(&availableLayerCount, availableLayers.data());

    // Verify all required layers are available.
    for (unsigned int i = 0; i < requiredValidationLayerCount; ++i)
    {
        bool found = false;
        for (unsigned int j = 0; j < availableLayerCount; ++j)
        {
            if (std::strcmp(requiredValidationLayers[i], availableLayers[j].layerName) == 0)
            {
                found = true;
                JE_CORE_INFO("Found validation layer: {0}...", requiredValidationLayers[i]);
                break;
            }
        }

        if (!found)
        {
            JE_CORE_FATAL("Required validation layer is missing: {0}", requiredValidationLayers[i]);
        }
    }

    createInfo.enabledLayerCount = requiredValidationLayers.size();
    createInfo.ppEnabledLayerNames = requiredValidationLayers.data();

    VkResult result = vkCreateInstance(&createInfo, nullptr, &m_Instance);
}
void VulkanContext::SwapBuffers()
{
    JE_CORE_ASSERT(true, "Implement this!");
}
}  // namespace JuicyEngine