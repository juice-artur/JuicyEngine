#include "VulkanContext.h"
#include "vulkan/vulkan.h"
#include <vulkan/vk_enum_string_helper.h>
#include "jepch.h"
#include "VulkanShader.h"
#include "Core/Core.h"

namespace JuicyEngine
{
VulkanContext::VulkanContext() : Instance(nullptr), DebugMessenger(nullptr)
{
}

void VulkanContext::Init(void* Window)
{
    InitInstance();
    SetupDebugMessenger();
    Surface = new VulkanSurface(Instance);
    Surface->Init(Window);
    Device = new VulkanDevice(Instance, Surface->GetSurface());

    SwapChain.Init(Device->GetPhysicalDevice(), Device->GetLogicalDevice(), Surface->GetSurface(), Window);

    CreateGraphicsPipeline();
}

void VulkanContext::SwapBuffers()
{
}

void VulkanContext::Shutdown()
{
    SwapChain.Shutdown(Device->GetLogicalDevice());
    delete Device;
    
    DestroyDebugUtilsMessengerEXT(Instance, DebugMessenger, nullptr);
    Surface->Shutdown();
    delete Surface;
    
    vkDestroyInstance(Instance, nullptr);
}

void VulkanContext::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& DebugCreateInfo)
{
    DebugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    DebugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                      VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    DebugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                                  VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    DebugCreateInfo.pfnUserCallback = &VulkanContext::DebugCallback;
    DebugCreateInfo.pUserData = nullptr;
    DebugCreateInfo.pNext = nullptr;
    DebugCreateInfo.flags = 0;
}

bool VulkanContext::InitInstance()
{
    VkApplicationInfo AppInfo{};
    AppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    //TODO: Set properly app name
    AppInfo.pApplicationName = "TODO";
    AppInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    AppInfo.pEngineName = "JuicyEngine";
    AppInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    AppInfo.apiVersion = VK_API_VERSION_1_3;

    VkDebugUtilsMessengerCreateInfoEXT DebugCreateInfo{};
    PopulateDebugMessengerCreateInfo(DebugCreateInfo);

    std::vector InstanceLayers = {
        "VK_LAYER_KHRONOS_validation",
    };

    JE_ASSERT(CheckValidationLayerSupport(InstanceLayers), "Validation layers requested, but not available!");

    std::vector InstanceExtensions = {
        VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
        VK_KHR_SURFACE_EXTENSION_NAME,
        "VK_KHR_win32_surface"
    };

    VkInstanceCreateInfo InstanceCreateInfo = {};
    InstanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    InstanceCreateInfo.pNext = &DebugCreateInfo;
    InstanceCreateInfo.flags = 0;
    InstanceCreateInfo.pApplicationInfo = &AppInfo;
    InstanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(InstanceLayers.size());
    InstanceCreateInfo.ppEnabledLayerNames = InstanceLayers.data();
    InstanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(InstanceExtensions.size());
    InstanceCreateInfo.ppEnabledExtensionNames = InstanceExtensions.data();

    VkResult CreateInstanceResult = vkCreateInstance(&InstanceCreateInfo, nullptr, &Instance);

    JE_CORE_ASSERT(CreateInstanceResult == VK_SUCCESS, "Failed to create vulkan Instance: {0}", string_VkResult(CreateInstanceResult));

    return true;
}

void VulkanContext::SetupDebugMessenger()
{
    VkDebugUtilsMessengerCreateInfoEXT CreateInfo;
    PopulateDebugMessengerCreateInfo(CreateInfo);
    if (CreateDebugUtilsMessengerEXT(Instance, &CreateInfo, nullptr, &DebugMessenger) != VK_SUCCESS)
    {
        JE_CORE_ASSERT(false, "Failed to set up debug messenger!");
    }
}

bool VulkanContext::CheckValidationLayerSupport(const std::vector<const char*>& ValidationLayers)
{
    uint32_t LayerCount;
    vkEnumerateInstanceLayerProperties(&LayerCount, nullptr);

    std::vector<VkLayerProperties> AvailableLayers(LayerCount);
    vkEnumerateInstanceLayerProperties(&LayerCount, AvailableLayers.data());

    for (const char* LayerName : ValidationLayers) {
        bool bFound = false;

        for (const auto& LayerProperties : AvailableLayers) {
            if (strcmp(LayerName, LayerProperties.layerName) == 0) {
                bFound = true;
                break;
            }
        }

        if (!bFound) {
            return false;
        }
    }

    return true;
}

VkResult VulkanContext::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
{
    auto Func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (Func != nullptr)
    {
        return Func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }
    else
    {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void VulkanContext::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
    const VkAllocationCallbacks* pAllocator)
{
    auto Func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (Func != nullptr)
    {
        Func(instance, debugMessenger, pAllocator);
    }
}

void VulkanContext::CreateGraphicsPipeline()
{
    auto VertShaderCode = VulkanShader::ReadFile("Assets/Shaders/vert.spv");
    auto FragShaderCode = VulkanShader::ReadFile("Assets/Shaders/frag.spv");
    
    VkShaderModule VertShaderModule = VulkanShader::CreateShaderModule(Device->GetLogicalDevice(), VertShaderCode);
    VkShaderModule FragShaderModule = VulkanShader::CreateShaderModule(Device->GetLogicalDevice(), FragShaderCode);
    
    VkPipelineShaderStageCreateInfo VertShaderStageInfo{};
    VertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    VertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    VertShaderStageInfo.module = VertShaderModule;
    VertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = FragShaderModule;
    fragShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo ShaderStages[] = {VertShaderStageInfo, fragShaderStageInfo};
    
    vkDestroyShaderModule(Device->GetLogicalDevice(), FragShaderModule, nullptr);
    vkDestroyShaderModule(Device->GetLogicalDevice(), VertShaderModule, nullptr);
}

VkBool32 VulkanContext::DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData)
{
    switch (messageSeverity)
    {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            JE_CORE_TRACE("[VULKAN] {0}", pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            JE_CORE_INFO("[VULKAN] {0}", pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            JE_CORE_WARN("[VULKAN] {0}", pCallbackData->pMessage);
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            JE_CORE_ERROR("[VULKAN] {0}", pCallbackData->pMessage);
            break;
        default: JE_CORE_ASSERT(false, "Incorrect enum value");
    }
    
    return VK_FALSE;
}
}