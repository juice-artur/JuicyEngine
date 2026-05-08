#include "VulkanDeviceManager.h"
#include "VulkanDevice.h"
#include "VulkanSurface.h"
#include "jepch.h"
#include "Core/Core.h"
#include <vulkan/vk_enum_string_helper.h>

namespace JuicyEngine
{

VulkanDeviceManager::VulkanDeviceManager() : m_Instance(VK_NULL_HANDLE), m_DebugMessenger(VK_NULL_HANDLE) {}

VulkanDeviceManager::~VulkanDeviceManager() {}

void VulkanDeviceManager::Init(void* WindowHandle)
{
    CreateInstance();
    SetupDebugMessenger();
    CreateSurface(WindowHandle);
    CreateDevice();
}

void VulkanDeviceManager::Shutdown()
{
    delete m_Device;
    m_Device = nullptr;

    m_Surface->Shutdown();
    delete m_Surface;
    m_Surface = nullptr;

    DestroyDebugUtilsMessengerEXT(m_Instance, m_DebugMessenger, nullptr);

    vkDestroyInstance(m_Instance, nullptr);
}

void VulkanDeviceManager::CreateInstance()
{
    VkApplicationInfo AppInfo = {.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
                                 .pNext = nullptr,
                                 .pApplicationName = "JuicyEngine",
                                 .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
                                 .pEngineName = "JuicyEngine",
                                 .engineVersion = VK_MAKE_VERSION(1, 0, 0),
                                 .apiVersion = VK_API_VERSION_1_3};

    VkDebugUtilsMessengerCreateInfoEXT DebugCreateInfo {};
    PopulateDebugMessengerCreateInfo(DebugCreateInfo);

    std::vector<const char*> InstanceLayers = {
        "VK_LAYER_KHRONOS_validation",
    };

    JE_ASSERT(CheckValidationLayerSupport(InstanceLayers), "Validation layers requested, but not available!")

    std::vector<const char*> InstanceExtensions = {
        VK_EXT_DEBUG_UTILS_EXTENSION_NAME, VK_KHR_SURFACE_EXTENSION_NAME, "VK_KHR_win32_surface"};

    VkInstanceCreateInfo InstanceCreateInfo = {.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
                                               .pNext = &DebugCreateInfo,
                                               .flags = 0,
                                               .pApplicationInfo = &AppInfo,
                                               .enabledLayerCount = static_cast<uint32_t>(InstanceLayers.size()),
                                               .ppEnabledLayerNames = InstanceLayers.data(),
                                               .enabledExtensionCount =
                                                   static_cast<uint32_t>(InstanceExtensions.size()),
                                               .ppEnabledExtensionNames = InstanceExtensions.data()};

    VkResult CreateInstanceResult = vkCreateInstance(&InstanceCreateInfo, nullptr, &m_Instance);

    JE_CORE_ASSERT(CreateInstanceResult == VK_SUCCESS,
                   "Failed to create vulkan Instance: {0}",
                   string_VkResult(CreateInstanceResult))
}

void VulkanDeviceManager::SetupDebugMessenger()
{
    VkDebugUtilsMessengerCreateInfoEXT CreateInfo;
    PopulateDebugMessengerCreateInfo(CreateInfo);
    if (CreateDebugUtilsMessengerEXT(m_Instance, &CreateInfo, nullptr, &m_DebugMessenger) != VK_SUCCESS)
    {
        JE_CORE_ASSERT(false, "Failed to set up debug messenger!")
    }
}

void VulkanDeviceManager::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& DebugCreateInfo)
{
    DebugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    DebugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                      VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                      VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    DebugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                                  VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                  VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    DebugCreateInfo.pfnUserCallback = &VulkanDeviceManager::DebugCallback;
    DebugCreateInfo.pUserData = nullptr;
    DebugCreateInfo.pNext = nullptr;
    DebugCreateInfo.flags = 0;
}

VkBool32 VulkanDeviceManager::DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT MessageSeverity,
                                            VkDebugUtilsMessageTypeFlagsEXT messageType,
                                            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                            void* pUserData)
{
    switch (MessageSeverity)
    {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            JE_CORE_TRACE("[VULKAN] {0}", pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT: JE_CORE_INFO("[VULKAN] {0}", pCallbackData->pMessage); break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            JE_CORE_WARN("[VULKAN] {0}", pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            JE_CORE_ERROR("[VULKAN] {0}", pCallbackData->pMessage);
            break;
        default: JE_CORE_ASSERT(false, "Incorrect enum value")
    }

    return VK_FALSE;
}

bool VulkanDeviceManager::CheckValidationLayerSupport(const std::vector<const char*>& ValidationLayers)
{
    uint32_t LayerCount;
    vkEnumerateInstanceLayerProperties(&LayerCount, nullptr);

    std::vector<VkLayerProperties> AvailableLayers(LayerCount);
    vkEnumerateInstanceLayerProperties(&LayerCount, AvailableLayers.data());

    for (const char* LayerName : ValidationLayers)
    {
        bool bFound = false;

        for (const auto& LayerProperties : AvailableLayers)
        {
            if (strcmp(LayerName, LayerProperties.layerName) == 0)
            {
                bFound = true;
                break;
            }
        }

        if (!bFound)
        {
            return false;
        }
    }

    return true;
}

VkResult VulkanDeviceManager::CreateDebugUtilsMessengerEXT(VkInstance instance,
                                                           const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                                                           const VkAllocationCallbacks* pAllocator,
                                                           VkDebugUtilsMessengerEXT* pDebugMessenger)
{
    PFN_vkCreateDebugUtilsMessengerEXT Func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
        vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));
    if (Func != nullptr)
    {
        return Func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }
    else
    {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void VulkanDeviceManager::DestroyDebugUtilsMessengerEXT(VkInstance instance,
                                                        VkDebugUtilsMessengerEXT debugMessenger,
                                                        const VkAllocationCallbacks* pAllocator)
{
    PFN_vkDestroyDebugUtilsMessengerEXT Func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
        vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"));
    if (Func != nullptr)
    {
        Func(instance, debugMessenger, pAllocator);
    }
}

void VulkanDeviceManager::CreateSurface(void* WindowHandle)
{
    m_Surface = new VulkanSurface(m_Instance);
    m_Surface->Init(WindowHandle);
}

void VulkanDeviceManager::CreateDevice()
{
    m_Device = new VulkanDevice(m_Instance, m_Surface->GetSurface());
}

} // namespace JuicyEngine
