#pragma once

#include "VulkanCommandBuffer.h"
#include "VulkanDevice.h"
#include "VulkanPipeline.h"
#include "VulkanRenderPass.h"
#include "VulkanSurface.h"
#include "VulkanSwapChain.h"
#include "Renderer/GraphicsContext.h"
#include "Renderer/Mesh.h"
#include "vulkan/vulkan.h"

namespace JuicyEngine
{
	class IndexBuffer;
	class VertexBuffer;
	
	class VulkanContext : public GraphicsContext
	{
	public:
		VulkanContext();

		virtual void Init(void* Window) override;
		virtual void SwapBuffers() override;
		virtual void Shutdown() override;
		void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& DebugCreateInfo);
		virtual void Draw() override;

		VulkanDevice* GetDevice() const;

		VkCommandPool GetCommandPool() const;

		// Disable copy/move
		VulkanContext(const VulkanContext&) = delete;
		VulkanContext& operator=(const VulkanContext&) = delete;
		VulkanContext(VulkanContext&&) = delete;
		VulkanContext& operator=(VulkanContext&&) = delete;

		~VulkanContext() override;

	private:
		bool InitInstance();
		void SetupDebugMessenger();
		bool CheckValidationLayerSupport(const std::vector<const char*>& ValidationLayers);

		VkResult CreateDebugUtilsMessengerEXT(VkInstance instance,
		                                      const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
		                                      const VkAllocationCallbacks* pAllocator,
		                                      VkDebugUtilsMessengerEXT* pDebugMessenger);
		void DestroyDebugUtilsMessengerEXT(VkInstance instance,
		                                   VkDebugUtilsMessengerEXT debugMessenger,
		                                   const VkAllocationCallbacks* pAllocator);

		void CreateGraphicsPipeline();

		void RecordCommandBuffer();

		void CreateSyncObjects();

		static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
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

		void* WindowPtr = nullptr;

		bool Skip = false;

		const std::vector<Vertex> Vertices = {
            {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
            {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
            {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
            {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
        };

		const std::vector<uint16_t> Indices = {
			0, 1, 2, 2, 3, 0
		};

		std::unique_ptr<VertexBuffer> VertexBuffer;
		std::unique_ptr<IndexBuffer> IndexBuffer;
	};
} // namespace JuicyEngine