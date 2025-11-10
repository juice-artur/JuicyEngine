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

/**
 * Construct a VulkanContext and initialize internal members to defaults.
 */
 
/**
 * Initialize the Vulkan context for the given native window handle.
 * @param Window Native window handle used to create the surface and swap chain.
 */
 
/**
 * Present the current image to the display and advance the swap chain.
 */
 
/**
 * Release all Vulkan resources owned by this context and reset internal state.
 */
 
/**
 * Fill the provided VkDebugUtilsMessengerCreateInfoEXT structure with the
 * application's preferred debug messenger settings.
 * @param DebugCreateInfo Structure to populate with debug messenger creation parameters.
 */
 
/**
 * Record and submit rendering commands for a single frame.
 */
 
/**
 * Return the VulkanDevice associated with this context.
 * @returns Pointer to the VulkanDevice used by this context, or nullptr if not initialized.
 */
 
/**
 * Return the Vulkan command pool used to allocate command buffers.
 * @returns VkCommandPool handle, or VK_NULL_HANDLE if no pool has been created.
 */
 
/**
 * Destroying and copying of VulkanContext is disabled; copy constructor is deleted.
 */
 
/**
 * Assignment of VulkanContext is disabled; copy assignment operator is deleted.
 */
 
/**
 * Move construction of VulkanContext is disabled; move constructor is deleted.
 */
 
/**
 * Move assignment of VulkanContext is disabled; move assignment operator is deleted.
 */
 
/**
 * Destroy the VulkanContext and free any remaining resources.
 */
 
/**
 * Create the Vulkan instance and initialize core Vulkan state.
 * @returns `true` if the Vulkan instance was created successfully, `false` otherwise.
 */
 
/**
 * Create and register the Vulkan debug messenger according to the populated
 * create-info previously provided.
 */
 
/**
 * Check whether all requested validation layers are available on the system.
 * @param ValidationLayers List of validation layer names to verify.
 * @returns `true` if every layer in `ValidationLayers` is supported, `false` otherwise.
 */
 
/**
 * Create a VkDebugUtilsMessengerEXT using the supplied parameters.
 * @param instance Vulkan instance to create the messenger for.
 * @param pCreateInfo Pointer to the creation parameters.
 * @param pAllocator Optional custom allocator, or nullptr to use the default allocator.
 * @param pDebugMessenger Output pointer that will receive the created messenger handle.
 * @returns VkResult indicating success or the type of failure.
 */
 
/**
 * Destroy a previously created VkDebugUtilsMessengerEXT.
 * @param instance Vulkan instance the messenger was created with.
 * @param debugMessenger Messenger handle to destroy.
 * @param pAllocator Optional custom allocator used when creating the messenger, or nullptr.
 */
 
/**
 * Create the graphics pipeline used for rendering, including shader stages,
 * pipeline layout, and fixed-function state.
 */
 
/**
 * Record draw and state commands into the active command buffer.
 */
 
/**
 * Create synchronization primitives (semaphores and fences) required for frame submission.
 */
 
/**
 * Debug callback invoked by Vulkan's validation layers for logged messages.
 * @param messageSeverity Severity flags for the message.
 * @param messageType Type flags for the message.
 * @param pCallbackData Detailed callback data provided by Vulkan.
 * @param pUserData User-supplied pointer provided when the messenger was created.
 * @returns `VK_TRUE` if the call should be considered handled and the Vulkan call should be aborted, `VK_FALSE` otherwise.
 */
namespace JuicyEngine
{
	class VulkanVertexBuffer;
	class VulkanIndexBuffer;
	
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

		std::unique_ptr<VulkanVertexBuffer> VertexBuffer;
		std::unique_ptr<VulkanIndexBuffer> IndexBuffer;
	};
} // namespace JuicyEngine