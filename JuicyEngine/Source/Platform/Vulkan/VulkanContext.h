#pragma once

#include "VulkanDevice.h"
#include "VulkanSurface.h"
#include "VulkanSwapChain.h"
#include "Renderer/GraphicsContext.h"
#include "vulkan/vulkan.h"

namespace JuicyEngine
{
class VulkanContext : public GraphicsContext
{
public:
    VulkanContext();
    
    virtual void Init(void* Window) override;
    virtual void SwapBuffers() override;
    virtual void Shutdown() override;
    void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& DebugCreateInfo);

private:
    bool InitInstance();
    void SetupDebugMessenger();
    bool CheckValidationLayerSupport(const std::vector<const char*>& ValidationLayers);
    
    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
    void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);

    void CreateGraphicsPipeline();
    
    static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(   VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData);
private:
    VkInstance Instance;
    VkDebugUtilsMessengerEXT DebugMessenger;

    VulkanDevice* Device;
    VulkanSurface* Surface;
    VulkanSwapChain SwapChain;;
};
}