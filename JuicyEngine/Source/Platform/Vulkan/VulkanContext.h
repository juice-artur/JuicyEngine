#pragma once

#include <memory>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE

#include "VulkanCommandBuffer.h"
#include "VulkanDeviceManager.h"
#include "VulkanPipeline.h"
#include "VulkanRenderPass.h"
#include "VulkanSwapChain.h"
#include "VulkanTexture2D.h"
#include "Renderer/GraphicsContext.h"
#include "Renderer/Mesh.h"
#include "vulkan/vulkan.h"

namespace JuicyEngine
{
class VulkanUniformBuffer;
class VulkanIndexBuffer;
class VulkanVertexBuffer;
class IndexBuffer;
class VertexBuffer;

class VulkanContext : public GraphicsContext
{
public:
    VulkanContext();

    virtual void Init(void* Window) override;
    virtual void SwapBuffers() override;
    virtual void Shutdown() override;
    virtual void Draw() override;

    VulkanDevice* GetDevice() const;
    VkInstance GetInstance() const;
    VulkanSwapChain& GetSwapchain();
    std::shared_ptr<VulkanRenderPass> GetRenderPass() const;
    VkRenderPass GetImGuiRenderPass() const;

    VkCommandPool GetCommandPool() const;

    // Disable copy/move
    VulkanContext(const VulkanContext&) = delete;
    VulkanContext& operator=(const VulkanContext&) = delete;
    VulkanContext(VulkanContext&&) = delete;
    VulkanContext& operator=(VulkanContext&&) = delete;

    ~VulkanContext() override;

    VkFormat FindDepthFormat();

    VkImageView GetDepthImageView() const;

    VkImageView GetViewportImageView() const;
    VkSampler GetViewportSampler() const;
    VkDescriptorSet GetViewportDescriptorSet() const;
    void ResizeViewportFramebuffer(uint32_t Width, uint32_t Height);
    void CreateViewportRenderPass();
    void CreateViewportFramebuffer(uint32_t Width, uint32_t Height);
    void DestroyViewportFramebuffer();
    VkRenderPass GetViewportRenderPass() const;
    VkFramebuffer GetViewportFramebuffer() const;
    VkExtent2D GetViewportSize() const;

private:
    void CreateGraphicsPipeline();
    void CreateViewportGraphicsPipeline();

    void CreateImGuiRenderPass();
    void CreateImGuiFramebuffers();
    void DestroyImGuiFramebuffers();

    void RecordCommandBuffer(VulkanRenderCommandBuffer& CB);

    void RecreateSwapChain();

    void CreateSyncObjects();

    void CreateDescriptorPool();
    void CreateDescriptorSets();

    void CreateDepthResources();
    VkFormat
    FindSupportedFormat(const std::vector<VkFormat>& Candidates, VkImageTiling Tiling, VkFormatFeatureFlags Features);

private:
    VulkanDeviceManager* m_DeviceManager;
    VulkanSwapChain SwapChain;
    VulkanPipeline GraphicsPipeline;
    VulkanPipeline ViewportGraphicsPipeline;
    std::shared_ptr<VulkanRenderPass> RenderPass = std::make_shared<VulkanRenderPass>();
    VkCommandPool CommandPool = VK_NULL_HANDLE;
    VkDescriptorSet DescriptorSet;
    std::vector<VulkanRenderCommandBuffer> CommandBuffers;

    std::unique_ptr<VulkanImage> DepthImage;

    static constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 2;
    std::vector<VkSemaphore> ImageAvailableSemaphores;
    std::vector<VkSemaphore> RenderFinishedSemaphores;
    std::vector<VkFence> GraphicsFences;
    uint32_t CurrentFrameIndex = 0;
    uint32_t CurrentSyncIndex = 0;
    uint32_t CurrentAcquireIndex = 0;
    VkDescriptorPool DescriptorPool;
    VkRenderPass ImGuiRenderPass = VK_NULL_HANDLE;
    std::vector<VkFramebuffer> ImGuiFramebuffers;
    VkRenderPass ViewportRenderPass = VK_NULL_HANDLE;
    VkFramebuffer ViewportFramebuffer = VK_NULL_HANDLE;
    std::unique_ptr<VulkanImage> ViewportImage;
    VkSampler ViewportSampler;
    VkDescriptorSet ViewportDescriptorSet = VK_NULL_HANDLE;
    std::unique_ptr<VulkanImage> ViewportDepthImage;
    VkExtent2D ViewportSize = {0, 0};
    void* WindowPtr = nullptr;

    bool Skip = false;
    bool FramebufferResized = false;

    const std::vector<Vertex> Vertices = {{{-2.0f, -2.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
                                          {{2.0f, -2.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
                                          {{2.0f, 2.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
                                          {{-2.0f, 2.0f, 0.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},

                                          {{-2.0f, -2.0f, -2.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
                                          {{2.0f, -2.0f, -2.0f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
                                          {{2.0f, 2.0f, -2.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
                                          {{-2.0f, 2.0f, -2.0f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}};

    const std::vector<uint16_t> Indices = {0, 1, 2, 2, 3, 0, 4, 5, 6, 6, 7, 4};

    UniformBufferObject Ubo {};

    std::unique_ptr<VulkanVertexBuffer> VertexBuffer;
    std::unique_ptr<VulkanIndexBuffer> IndexBuffer;
    std::unique_ptr<VulkanUniformBuffer> UniformBuffer;
    std::unique_ptr<VulkanTexture2D> Texture;
};
} // namespace JuicyEngine