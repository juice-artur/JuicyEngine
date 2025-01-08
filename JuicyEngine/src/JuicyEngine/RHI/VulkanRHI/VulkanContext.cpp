#include "jepch.h"
#include "VulkanContext.h"
#include "Platform/VulkanPlatform.h"

namespace JuicyEngine
{
VKAPI_ATTR VkBool32 VKAPI_CALL vkDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageTypes, const VkDebugUtilsMessengerCallbackDataEXT* callbackData, void* userData)
{
    switch (messageSeverity)
    {
        default:
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT: JE_ERROR(callbackData->pMessage); break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: JE_WARN(callbackData->pMessage); break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT: JE_INFO(callbackData->pMessage); break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: JE_TRACE(callbackData->pMessage); break;
    }
    return VK_FALSE;
}

VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr)
    {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }
    else
    {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr)
    {
        func(instance, debugMessenger, pAllocator);
    }
}

JuicyEngine::VulkanContext::VulkanContext(Window* windowHandle) : m_Window(windowHandle)
{
    JE_CORE_ASSERT(windowHandle, "Window handle is null!");
}
VulkanContext::~VulkanContext()
{
    m_Device.Cleanup();  
    DestroyDebugUtilsMessengerEXT(m_Instance, debugMessenger, nullptr);
    vkDestroyInstance(m_Instance, nullptr);
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
    extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);

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

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    PopulateDebugMessengerCreateInfo(debugCreateInfo);
    createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;

    VkResult result = vkCreateInstance(&createInfo, nullptr, &m_Instance);

    SetupDebugMessenger();

    m_Device = VulkanDevice(&m_Instance);
    m_Device.Init();


}
void VulkanContext::SwapBuffers()
{
    JE_CORE_ASSERT(true, "Implement this!");
}
void VulkanContext::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = vkDebugCallback;
}
void VulkanContext::SetupDebugMessenger()
{
    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    PopulateDebugMessengerCreateInfo(createInfo);
    if (CreateDebugUtilsMessengerEXT(m_Instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS)
    {
        JE_CORE_FATAL("Failed to set up debug messenger!");
    }
}
}  // namespace JuicyEngine