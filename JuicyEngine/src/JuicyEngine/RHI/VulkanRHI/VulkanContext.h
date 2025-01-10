#pragma once

#include "JuicyEngine/Renderer/GraphicsContext.h"
#include "JuicyEngine/Core/Window.h"
#include "vulkan/vulkan.h"
#include "VulkanDevice.h"
#include "VulkanSwapchain.h"

namespace JuicyEngine
{
class VulkanContext : public GraphicsContext
{
public:
    VulkanContext(Window* windowHandle);
    virtual ~VulkanContext() override;
    // Inherited via GraphicsContext
    void Init() override;
    void SwapBuffers() override;

private:
    void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
    void SetupDebugMessenger();
    void CreateRenderPass();
    void CreateFramebuffers();

private:
    Window* m_Window;

    VkInstance m_Instance;
    VkDebugUtilsMessengerEXT debugMessenger;

    VulkanDevice m_Device;
    VkSurfaceKHR m_Surface = VK_NULL_HANDLE;

    VulkanSwapchain* m_Swapchain;

    VkRenderPass renderPass;
    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;
    std::vector<VkFramebuffer> swapChainFramebuffers;
};
}  // namespace JuicyEngine