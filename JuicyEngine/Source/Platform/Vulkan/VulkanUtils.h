#pragma once
#include <vulkan/vulkan.h>

namespace  JuicyEngine::RHI::Vulkan
{
	class VulkanUtils
	{
	public:
		static uint32_t FindMemoryType(uint32_t TypeFilter, VkMemoryPropertyFlags Properties);
		static VkCommandBuffer BeginSingleTimeCommands();
		static void EndSingleTimeCommands(VkCommandBuffer VkCommandBuffer);
		static void CreateBuffer(VkDeviceSize Size,
								VkBufferUsageFlags Usage,
								VkMemoryPropertyFlags Properties,
								VkBuffer& InBuffer,
								VkDeviceMemory& InBufferMemory);
		
	};
}