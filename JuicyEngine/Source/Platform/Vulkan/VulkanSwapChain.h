#pragma once

#include "jepch.h"
#include "vulkan/vulkan.h"

namespace JuicyEngine
{
	struct SwapChainSupportDetails
	{
		VkSurfaceCapabilitiesKHR Capabilities;
		std::vector<VkSurfaceFormatKHR> Formats;
		std::vector<VkPresentModeKHR> PresentModes;
	};

	class VulkanSwapChain
	{
	public:
		VulkanSwapChain();
		void Shutdown() const;

		void Init(VkSurfaceKHR Surface, void* Window);
		static SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice Device, VkSurfaceKHR Surface);
		VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& AvailableFormats);
		VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& AvailablePresentModes);
		VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& Capabilities, void* Window);
		VkFormat GetFormat() const;
		VkExtent2D GetExtent() const;
		std::vector<VkImageView>& GetSwapChainImageViews();
		VkSwapchainKHR GetSwapChain() const;

	private:
		void CreateImageViews(const VkDevice Device);

	private:
		VkSwapchainKHR SwapChain;
		uint32_t ImageCount;
		std::vector<VkImage> SwapChainImages;
		VkFormat SwapChainImageFormat;
		VkExtent2D SwapChainExtent;
		std::vector<VkImageView> SwapChainImageViews;
	};
} // namespace JuicyEngine
