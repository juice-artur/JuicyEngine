#include "VulkanCommandBuffer.h"

#include "VulkanContext.h"
#include "Core/Core.h"

namespace JuicyEngine
{
void VulkanRenderCommandBuffer::Init(VkDevice Device, VkCommandPool InCommandPool)
{
    VkCommandBufferAllocateInfo AllocInfo {.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
                                           .pNext = VK_NULL_HANDLE,
                                           .commandPool = InCommandPool,
                                           .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                                           .commandBufferCount = 1};

    auto Result = vkAllocateCommandBuffers(Device, &AllocInfo, &CommandBuffer);
    JE_CORE_ASSERT(Result == VK_SUCCESS, "Failed to allocate command buffers!")
    CommandPool = InCommandPool;
}

void VulkanRenderCommandBuffer::Begin()
{
    VkCommandBufferBeginInfo cmdBufBeginInfo = {};
    cmdBufBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    cmdBufBeginInfo.flags = 0;

    auto Result = vkBeginCommandBuffer(CommandBuffer, &cmdBufBeginInfo);
    JE_CORE_ASSERT(Result == VK_SUCCESS, "Failed to begin recording command buffer!")
}

void VulkanRenderCommandBuffer::End()
{
    auto Result = vkEndCommandBuffer(CommandBuffer);
    JE_CORE_ASSERT(Result == VK_SUCCESS, "Failed to End recording command buffer!")
}

void VulkanRenderCommandBuffer::Reset()
{
    vkResetCommandBuffer(CommandBuffer, 0);
}

VkCommandBuffer& VulkanRenderCommandBuffer::GetCommandBuffer()
{
    return CommandBuffer;
}
} // namespace JuicyEngine