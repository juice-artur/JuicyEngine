#include "VulkanCommandBuffer.h"

#include "Core/Core.h"

namespace JuicyEngine
{
	void VulkanRenderCommandBuffer::Init(VkDevice Device, VkCommandPool CommandPool)
	{
		VkCommandBufferAllocateInfo AllocInfo {};
		AllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		AllocInfo.commandPool = CommandPool;
		AllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		AllocInfo.commandBufferCount = 1;

		auto Result = vkAllocateCommandBuffers(Device, &AllocInfo, &CommandBuffer);
		JE_CORE_ASSERT(Result == VK_SUCCESS, "Failed to allocate command buffers!");
	}

	void VulkanRenderCommandBuffer::Begin()
	{
		auto ResetResult = vkResetCommandBuffer(CommandBuffer, 0);
		JE_CORE_ASSERT(ResetResult == VK_SUCCESS, "Failed to reset recording command buffer!");

		VkCommandBufferBeginInfo cmdBufBeginInfo = {};
		cmdBufBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		cmdBufBeginInfo.flags = 0;

		auto Result = vkBeginCommandBuffer(CommandBuffer, &cmdBufBeginInfo);
		JE_CORE_ASSERT(Result == VK_SUCCESS, "Failed to begin recording command buffer!");
	}

	void VulkanRenderCommandBuffer::End()
	{
		auto Result = vkEndCommandBuffer(CommandBuffer);
		JE_CORE_ASSERT(Result == VK_SUCCESS, "Failed to End recording command buffer!");
	}

	VkCommandBuffer& VulkanRenderCommandBuffer::GetCommandBuffer()
	{
		return CommandBuffer;
	}
} // namespace JuicyEngine