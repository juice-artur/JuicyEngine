#pragma once

#include <vulkan/vulkan.h>
#include <vector>

#include "VulkanDevice.h"
#include "VulkanSurface.h"

namespace JuicyEngine
{

class VulkanDeviceManager
{
public:
    VulkanDeviceManager();
    ~VulkanDeviceManager();

    void Init(void* WindowHandle);
    void Shutdown();

    VkInstance GetInstance() const
    {
        return m_Instance;
    }
    VulkanDevice* GetDevice() const
    {
        return m_Device;
    }
    VulkanSurface* GetSurface() const
    {
        return m_Surface;
    }

    VulkanDeviceManager(const VulkanDeviceManager&) = delete;
    VulkanDeviceManager& operator=(const VulkanDeviceManager&) = delete;

private:
    void CreateInstance();
    void SetupDebugMessenger();
    void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& DebugCreateInfo);
    void CreateSurface(void* WindowHandle);
    void CreateDevice();

    static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT MessageSeverity,
                                                        VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                        void* pUserData);

    bool CheckValidationLayerSupport(const std::vector<const char*>& ValidationLayers);

    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance,
                                          const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                                          const VkAllocationCallbacks* pAllocator,
                                          VkDebugUtilsMessengerEXT* pDebugMessenger);
    void DestroyDebugUtilsMessengerEXT(VkInstance instance,
                                       VkDebugUtilsMessengerEXT debugMessenger,
                                       const VkAllocationCallbacks* pAllocator);

    VkInstance m_Instance = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT m_DebugMessenger = VK_NULL_HANDLE;
    VulkanDevice* m_Device = nullptr;
    VulkanSurface* m_Surface = nullptr;
};

} // namespace JuicyEngine
