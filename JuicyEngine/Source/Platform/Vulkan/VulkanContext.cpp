#include "VulkanContext.h"

#include "VulkanBuffer.h"
#include "jepch.h"
#include "vulkan/vulkan.h"
#include <vulkan/vk_enum_string_helper.h>
#include "VulkanShader.h"
#include "Core/Core.h"
#include "Renderer/Pipeline.h"

namespace JuicyEngine
{
	VulkanContext::VulkanContext() : Instance(nullptr), DebugMessenger(nullptr) {}

	void VulkanContext::Init(void* Window)
	{
		WindowPtr = Window;
		InitInstance();
		SetupDebugMessenger();
		Surface = new VulkanSurface(Instance);
		Surface->Init(Window);
		Device = new VulkanDevice(Instance, Surface->GetSurface());

		VkCommandPoolCreateInfo PoolInfo {};
		PoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		PoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		PoolInfo.queueFamilyIndex = Device->GetQueueFamilyIndices().GraphicsFamily.value();

		auto CreateCommandPoolResult
		    = vkCreateCommandPool(Device->GetLogicalDevice(), &PoolInfo, nullptr, &CommandPool);

		JE_CORE_ASSERT(CreateCommandPoolResult == VK_SUCCESS,
		               "Failed to create vulkan Instance: {0}",
		               string_VkResult(CreateCommandPoolResult))

		SwapChain.Init(Surface->GetSurface(), Window);
		RenderPass->CreateRenderPass(SwapChain.GetFormat(), SwapChain.GetSwapChainImageViews(), SwapChain.GetExtent());
		CommandBuffer.Init(Device->GetLogicalDevice(), CommandPool);
		CreateGraphicsPipeline();
		CreateSyncObjects();

		VertexBuffer.reset(VertexBuffer::Create(Vertices));
		IndexBuffer.reset(IndexBuffer::Create( Indices));
	}

	void VulkanContext::SwapBuffers()
	{
		if (Skip)
		{
			Skip = false;
			return;
		}

		VkSubmitInfo SubmitInfo {};
		SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphores[] = { ImageAvailableSemaphore };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		SubmitInfo.waitSemaphoreCount = 1;
		SubmitInfo.pWaitSemaphores = waitSemaphores;
		SubmitInfo.pWaitDstStageMask = waitStages;

		SubmitInfo.commandBufferCount = 1;
		SubmitInfo.pCommandBuffers = &CommandBuffer.GetCommandBuffer();

		VkSemaphore signalSemaphores[] = { RenderFinishedSemaphore };
		SubmitInfo.signalSemaphoreCount = 1;
		SubmitInfo.pSignalSemaphores = signalSemaphores;

		auto QueueSubmitResult = vkQueueSubmit(Device->GetGraphicsQueue(), 1, &SubmitInfo, InFlightFence);
		JE_ASSERT(QueueSubmitResult == VK_SUCCESS, "Queue submit failed!")

		VkPresentInfoKHR PresentInfo {};
		PresentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

		PresentInfo.waitSemaphoreCount = 1;
		PresentInfo.pWaitSemaphores = signalSemaphores;

		VkSwapchainKHR swapChains[] = { SwapChain.GetSwapChain() };
		PresentInfo.swapchainCount = 1;
		PresentInfo.pSwapchains = swapChains;

		PresentInfo.pImageIndices = &RenderPass->SwapChainImageIndex;

		vkQueuePresentKHR(Device->GetPresentQueue(), &PresentInfo);
	}

	void VulkanContext::Shutdown()
	{
		vkDeviceWaitIdle(Device->GetLogicalDevice());
		vkDestroySemaphore(Device->GetLogicalDevice(), ImageAvailableSemaphore, nullptr);
		vkDestroySemaphore(Device->GetLogicalDevice(), RenderFinishedSemaphore, nullptr);
		vkDestroyFence(Device->GetLogicalDevice(), InFlightFence, nullptr);
		vkDestroyCommandPool(Device->GetLogicalDevice(), CommandPool, nullptr);
		GraphicsPipeline.Shutdown();
		RenderPass->Shutdown();
		RenderPass.reset();
		VertexBuffer.reset();
		IndexBuffer.reset();
		SwapChain.Shutdown();
		delete Device;

		DestroyDebugUtilsMessengerEXT(Instance, DebugMessenger, nullptr);
		Surface->Shutdown();
		delete Surface;

		vkDestroyInstance(Instance, nullptr);
	}

	void VulkanContext::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& DebugCreateInfo)
	{
		DebugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		DebugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
		                                  | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
		                                  | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		DebugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
		                              | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
		                              | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		DebugCreateInfo.pfnUserCallback = &VulkanContext::DebugCallback;
		DebugCreateInfo.pUserData = nullptr;
		DebugCreateInfo.pNext = nullptr;
		DebugCreateInfo.flags = 0;
	}

	void VulkanContext::Draw()
	{
		vkWaitForFences(Device->GetLogicalDevice(), 1, &InFlightFence, VK_TRUE, UINT64_MAX);

		VkResult Result = vkAcquireNextImageKHR(Device->GetLogicalDevice(),
		                                        SwapChain.GetSwapChain(),
		                                        UINT64_MAX,
		                                        ImageAvailableSemaphore,
		                                        VK_NULL_HANDLE,
		                                        &RenderPass->SwapChainImageIndex);

		if (Result == VK_ERROR_OUT_OF_DATE_KHR || Result == VK_SUBOPTIMAL_KHR)
		{
			vkDeviceWaitIdle(Device->GetLogicalDevice());
			RenderPass->Shutdown();
			SwapChain.Shutdown();

			SwapChain.Init(Surface->GetSurface(), WindowPtr);
			RenderPass->CreateRenderPass(
			    SwapChain.GetFormat(), SwapChain.GetSwapChainImageViews(), SwapChain.GetExtent());

			Skip = true;
			return;
		}

		vkResetFences(Device->GetLogicalDevice(), 1, &InFlightFence);
		RecordCommandBuffer();
	}
	VulkanDevice* VulkanContext::GetDevice() const
	{
		return Device;
	}
	VkCommandPool VulkanContext::GetCommandPool() const
	{
		return CommandPool;
	}

	VulkanContext::~VulkanContext()
	{
		
	}
	
	bool VulkanContext::InitInstance()
	{
		VkApplicationInfo AppInfo = { .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
			                          .pNext = nullptr,
			                          .pApplicationName = "JuicyEngine",
			                          .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
			                          .pEngineName = "JuicyEngine",
			                          .engineVersion = VK_MAKE_VERSION(1, 0, 0),
			                          .apiVersion = VK_API_VERSION_1_3 };

		VkDebugUtilsMessengerCreateInfoEXT DebugCreateInfo {};
		PopulateDebugMessengerCreateInfo(DebugCreateInfo);

		std::vector<const char*> InstanceLayers = {
			"VK_LAYER_KHRONOS_validation",
		};

		JE_ASSERT(CheckValidationLayerSupport(InstanceLayers), "Validation layers requested, but not available!")

		std::vector<const char*> InstanceExtensions
		    = { VK_EXT_DEBUG_UTILS_EXTENSION_NAME, VK_KHR_SURFACE_EXTENSION_NAME, "VK_KHR_win32_surface" };

		VkInstanceCreateInfo InstanceCreateInfo
		    = { .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
			    .pNext = &DebugCreateInfo,
			    .flags = 0,
			    .pApplicationInfo = &AppInfo,
			    .enabledLayerCount = static_cast<uint32_t>(InstanceLayers.size()),
			    .ppEnabledLayerNames = InstanceLayers.data(),
			    .enabledExtensionCount = static_cast<uint32_t>(InstanceExtensions.size()),
			    .ppEnabledExtensionNames = InstanceExtensions.data() };

		VkResult CreateInstanceResult = vkCreateInstance(&InstanceCreateInfo, nullptr, &Instance);

		JE_CORE_ASSERT(CreateInstanceResult == VK_SUCCESS,
		               "Failed to create vulkan Instance: {0}",
		               string_VkResult(CreateInstanceResult))

		return true;
	}

	void VulkanContext::SetupDebugMessenger()
	{
		VkDebugUtilsMessengerCreateInfoEXT CreateInfo;
		PopulateDebugMessengerCreateInfo(CreateInfo);
		if (CreateDebugUtilsMessengerEXT(Instance, &CreateInfo, nullptr, &DebugMessenger) != VK_SUCCESS)
		{
			JE_CORE_ASSERT(false, "Failed to set up debug messenger!")
		}
	}

	bool VulkanContext::CheckValidationLayerSupport(const std::vector<const char*>& ValidationLayers)
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

	VkResult VulkanContext::CreateDebugUtilsMessengerEXT(VkInstance instance,
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

	void VulkanContext::DestroyDebugUtilsMessengerEXT(VkInstance instance,
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

	void VulkanContext::CreateGraphicsPipeline()
	{
		auto VertShaderCode = VulkanShader::ReadFile("Assets/Shaders/vert.spv");
		auto FragShaderCode = VulkanShader::ReadFile("Assets/Shaders/frag.spv");

		VkShaderModule VertShaderModule = VulkanShader::CreateShaderModule(Device->GetLogicalDevice(), VertShaderCode);
		VkShaderModule FragShaderModule = VulkanShader::CreateShaderModule(Device->GetLogicalDevice(), FragShaderCode);

		VkPipelineShaderStageCreateInfo VertShaderStageInfo {};
		VertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		VertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		VertShaderStageInfo.module = VertShaderModule;
		VertShaderStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo fragShaderStageInfo {};
		fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStageInfo.module = FragShaderModule;
		fragShaderStageInfo.pName = "main";

		PipelineCreateInfo Info = {};
		Info.RenderPass = RenderPass;
		Info.VulkanShaderModules = std::vector { VertShaderStageInfo, fragShaderStageInfo };
		GraphicsPipeline.Create(Info);

		vkDestroyShaderModule(Device->GetLogicalDevice(), FragShaderModule, nullptr);
		vkDestroyShaderModule(Device->GetLogicalDevice(), VertShaderModule, nullptr);
	}

	void VulkanContext::RecordCommandBuffer()
	{
		CommandBuffer.Begin();
		RenderPass->Begin(CommandBuffer.GetCommandBuffer(), SwapChain.GetExtent());
		GraphicsPipeline.Bind(CommandBuffer.GetCommandBuffer());

		VkViewport Viewport =
		{
			.x = 0.0f,
			.y = 0.0f,
			.width = static_cast<float>(SwapChain.GetExtent().width),
			.height = static_cast<float>(SwapChain.GetExtent().height),
			.minDepth = 0.0f,
			.maxDepth = 1.0f
		};
		
		vkCmdSetViewport(CommandBuffer.GetCommandBuffer(), 0, 1, &Viewport);

		VkRect2D Scissor { .offset = { 0, 0 }, .extent = SwapChain.GetExtent() };

		vkCmdSetScissor(CommandBuffer.GetCommandBuffer(), 0, 1, &Scissor);

		VkDeviceSize Offsets[] = { 0 };

		vkCmdBindVertexBuffers(CommandBuffer.GetCommandBuffer(), 0, 1, (VkBuffer*)VertexBuffer->GetNativeHandle(), Offsets);
		vkCmdBindIndexBuffer(CommandBuffer.GetCommandBuffer(), *(VkBuffer*)IndexBuffer->GetNativeHandle(), 0, VK_INDEX_TYPE_UINT16);
		
		vkCmdDrawIndexed(CommandBuffer.GetCommandBuffer(),  static_cast<uint32_t>(Indices.size()), 1, 0, 0, 0);
		RenderPass->End(CommandBuffer.GetCommandBuffer());
		CommandBuffer.End();
	}

	void VulkanContext::CreateSyncObjects()
	{
		VkSemaphoreCreateInfo semaphoreInfo {};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo {};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		if (vkCreateSemaphore(Device->GetLogicalDevice(), &semaphoreInfo, nullptr, &ImageAvailableSemaphore)
		        != VK_SUCCESS
		    || vkCreateSemaphore(Device->GetLogicalDevice(), &semaphoreInfo, nullptr, &RenderFinishedSemaphore)
		           != VK_SUCCESS
		    || vkCreateFence(Device->GetLogicalDevice(), &fenceInfo, nullptr, &InFlightFence) != VK_SUCCESS)
		{
			JE_CORE_ASSERT(false, "failed to create synchronization objects for a frame!")
		}
	}

	VkBool32 VulkanContext::DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	                                      VkDebugUtilsMessageTypeFlagsEXT messageType,
	                                      const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	                                      void* pUserData)
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
			default:
				JE_CORE_ASSERT(false, "Incorrect enum value")
		}

		return VK_FALSE;
	}
} // namespace JuicyEngine