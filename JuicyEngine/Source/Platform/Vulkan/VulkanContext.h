#pragma once

#include "VulkanCommandBuffer.h"
#include "VulkanDevice.h"
#include "VulkanPipeline.h"
#include "VulkanRenderPass.h"
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
    virtual void Draw() override;

private:
    bool InitInstance();
    void SetupDebugMessenger();
    bool CheckValidationLayerSupport(const std::vector<const char*>& ValidationLayers);
    
    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
    void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);

    void CreateGraphicsPipeline();

    void RecordCommandBuffer();

    void CreateSyncObjects();
    
    static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(   VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData);
private:
    VkInstance Instance;
    VkDebugUtilsMessengerEXT DebugMessenger;

    VulkanDevice* Device;
    VulkanSurface* Surface;
    VulkanSwapChain SwapChain;
    VulkanPipeline GraphicsPipeline;
    std::shared_ptr<VulkanRenderPass> RenderPass = std::make_shared<VulkanRenderPass>();
    VkCommandPool CommandPool = VK_NULL_HANDLE;

    VulkanRenderCommandBuffer CommandBuffer;

    VkSemaphore ImageAvailableSemaphore;
    VkSemaphore RenderFinishedSemaphore;
    VkFence InFlightFence;
};
}