#include "VulkanUtils.h"

#include "VulkanContext.h"
#include "Core/Core.h"

namespace JuicyEngine::RHI::Vulkan
{
	uint32_t VulkanUtils::FindMemoryType(uint32_t TypeFilter, VkMemoryPropertyFlags Properties)
	{
		const auto* Context = dynamic_cast<VulkanContext*>(VulkanContext::Get());

		VkPhysicalDeviceMemoryProperties MemProperties;
		vkGetPhysicalDeviceMemoryProperties(Context->GetDevice()->GetPhysicalDevice(), &MemProperties);

		for (uint32_t i = 0; i < MemProperties.memoryTypeCount; i++)
		{
			if ((TypeFilter & (1 << i)) && (MemProperties.memoryTypes[i].propertyFlags & Properties) == Properties)
			{
				return i;
			}
		}

		JE_CORE_ASSERT(false, "Failed to find suitable memory type!")
		return 0;
	}

	VkCommandBuffer VulkanUtils::BeginSingleTimeCommands()
	{
		const auto* Context = dynamic_cast<VulkanContext*>(VulkanContext::Get());

		VkCommandBufferAllocateInfo CmdBufAllocInfo = {};
		CmdBufAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		CmdBufAllocInfo.commandPool = Context->GetCommandPool();
		CmdBufAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		CmdBufAllocInfo.commandBufferCount = 1;

		VkCommandBuffer VkCommandBuffer;
		auto AllocResult
		    = vkAllocateCommandBuffers(Context->GetDevice()->GetLogicalDevice(), &CmdBufAllocInfo, &VkCommandBuffer);
		JE_CORE_ASSERT(AllocResult == VK_SUCCESS, "Failed to allocate command buffers!")

		VkCommandBufferBeginInfo CmdBufBeginInfo = {};
		CmdBufBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		CmdBufBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		auto BeginResult = vkBeginCommandBuffer(VkCommandBuffer, &CmdBufBeginInfo);
		JE_CORE_ASSERT(BeginResult == VK_SUCCESS, "Failed to begin command buffer!")

		return VkCommandBuffer;
	}

	void VulkanUtils::EndSingleTimeCommands(VkCommandBuffer VkCommandBuffer)
	{
		const auto* Context = dynamic_cast<VulkanContext*>(VulkanContext::Get());

		vkEndCommandBuffer(VkCommandBuffer);

		VkSubmitInfo SubmitInfo = {};
		SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		SubmitInfo.commandBufferCount = 1;
		SubmitInfo.pCommandBuffers = &VkCommandBuffer;

		const VkResult SubmitResult
		    = vkQueueSubmit(Context->GetDevice()->GetGraphicsQueue(), 1, &SubmitInfo, VK_NULL_HANDLE);
		JE_CORE_ASSERT(SubmitResult == VK_SUCCESS, "Failed to submit copy command buffer")

		vkQueueWaitIdle(Context->GetDevice()->GetGraphicsQueue());

		vkFreeCommandBuffers(Context->GetDevice()->GetLogicalDevice(), Context->GetCommandPool(), 1, &VkCommandBuffer);
	}
	void VulkanUtils::CreateBuffer(VkDeviceSize Size,
	                               VkBufferUsageFlags Usage,
	                               VkMemoryPropertyFlags Properties,
	                               VkBuffer& InBuffer,
	                               VkDeviceMemory& InBufferMemory)
	{
		VkBufferCreateInfo BufferInfo =
		{
			.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
			.pNext = nullptr,
			.flags = 0,
			.size = Size,
			.usage = Usage,
			.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
			.queueFamilyIndexCount = 0,
			.pQueueFamilyIndices = nullptr
		};

		const auto* Context = dynamic_cast<VulkanContext*>(VulkanContext::Get());
		const auto Result = vkCreateBuffer(Context->GetDevice()->GetLogicalDevice(), &BufferInfo, nullptr, &InBuffer);

		JE_CORE_ASSERT(Result == VK_SUCCESS, "Failed to create Vulkan buffer!")

		VkMemoryRequirements MemRequirements;

		vkGetBufferMemoryRequirements(Context->GetDevice()->GetLogicalDevice(), InBuffer, &MemRequirements);

		VkMemoryAllocateInfo AllocInfo = { .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
			                               .pNext = nullptr,
			                               .allocationSize = MemRequirements.size,
			                               .memoryTypeIndex = RHI::Vulkan::VulkanUtils::FindMemoryType(
			                                   MemRequirements.memoryTypeBits, Properties) };

		const auto AllocResult
		    = vkAllocateMemory(Context->GetDevice()->GetLogicalDevice(), &AllocInfo, nullptr, &InBufferMemory);

		JE_CORE_ASSERT(AllocResult == VK_SUCCESS, "Failed to allocate vertex buffer memory")

		vkBindBufferMemory(Context->GetDevice()->GetLogicalDevice(), InBuffer, InBufferMemory, 0);
	}
} // namespace JuicyEngine::RHI::Vulkan
