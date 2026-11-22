#pragma once
#include <vulkan/vulkan.h>

namespace JuicyEngine
{
class VulkanContext;
}

namespace JuicyEngine::RHI::Vulkan
{
class VulkanUtils
{
public:
    static uint32_t FindMemoryType(VulkanContext& Context, uint32_t TypeFilter, VkMemoryPropertyFlags Properties);
    static VkCommandBuffer BeginSingleTimeCommands(VulkanContext& Context);
    static void EndSingleTimeCommands(VulkanContext& Context, VkCommandBuffer VkCommandBuffer);
    static void CreateBuffer(VulkanContext& Context,
                             VkDeviceSize Size,
                             VkBufferUsageFlags Usage,
                             VkMemoryPropertyFlags Properties,
                             VkBuffer& InBuffer,
                             VkDeviceMemory& InBufferMemory);
};
} // namespace JuicyEngine::RHI::Vulkan
