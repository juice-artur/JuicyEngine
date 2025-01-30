#pragma once

#include "JuicyEngine/Renderer/GraphicsContext.h"
#include "JuicyEngine/Core/Window.h"
#include "vulkan/vulkan.h"
#include "VulkanDevice.h"
#include "VulkanSwapchain.h"

namespace JuicyEngine
{
const int MAX_FRAMES_IN_FLIGHT = 2;

class VulkanContext : public GraphicsContext
{
public:
    VulkanContext(Window* windowHandle);
    virtual ~VulkanContext() override;
    // Inherited via GraphicsContext
    void Init() override;
    void SwapBuffers() override;
    void DrawFrame();

private:
    void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
    void SetupDebugMessenger();
    void CreateRenderPass();
    void CreateFramebuffers();
    void CreateCommandPool();
    void CreateCommandBuffers();
    void RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
    void CreateSyncObjects();

private:
    Window* m_Window;

    VkInstance m_Instance;
    VkDebugUtilsMessengerEXT debugMessenger;

    VulkanDevice m_Device;
    VkSurfaceKHR m_Surface = VK_NULL_HANDLE;

    VulkanSwapchain* m_Swapchain;

    VkCommandPool commandPool;

    VkRenderPass renderPass;
    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;
    std::vector<VkFramebuffer> swapChainFramebuffers;
    std::vector<VkFence> inFlightFences;
    std::vector<VkCommandBuffer> commandBuffers;

    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    uint32_t currentFrame = 0;
};
}  // namespace JuicyEngine