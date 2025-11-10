#include "VulkanBuffer.h"

#include "VulkanContext.h"
#include "Core/Core.h"

namespace JuicyEngine
{
	uint32_t VulkanBuffer::FindMemoryType(uint32_t TypeFilter, VkMemoryPropertyFlags Properties)
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

	VkBuffer& VulkanBuffer::GetBuffer()
	{
		return Buffer;
	}

	VulkanBuffer::~VulkanBuffer()
	{
		const auto* Context = dynamic_cast<VulkanContext*>(VulkanContext::Get());
		vkDestroyBuffer(Context->GetDevice()->GetLogicalDevice(), Buffer, nullptr);
		vkFreeMemory(Context->GetDevice()->GetLogicalDevice(), BufferMemory, nullptr);
	}
	void VulkanBuffer::CreateBuffer(VkDeviceSize Size,
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

		VkMemoryAllocateInfo AllocInfo =
		{
			.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
			.pNext = nullptr,
			.allocationSize = MemRequirements.size,
			.memoryTypeIndex = FindMemoryType(MemRequirements.memoryTypeBits, Properties)
		};

		const auto AllocResult
		    = vkAllocateMemory(Context->GetDevice()->GetLogicalDevice(), &AllocInfo, nullptr, &InBufferMemory);

		JE_CORE_ASSERT(AllocResult == VK_SUCCESS, "Failed to allocate vertex buffer memory")

		vkBindBufferMemory(Context->GetDevice()->GetLogicalDevice(), InBuffer, InBufferMemory, 0);
	}
	
	void VulkanBuffer::CopyBuffer(VkBuffer SrcBuffer, VkBuffer DstBuffer, VkDeviceSize Size)
	{
		const auto* Context = dynamic_cast<VulkanContext*>(VulkanContext::Get());
		
		VkCommandBufferAllocateInfo AllocInfo =
		{
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
			.pNext = nullptr,
			.commandPool = Context->GetCommandPool(),
			.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
			.commandBufferCount = 1
		};

		VkCommandBuffer CommandBuffer;
		const VkResult AllocateResult =
			vkAllocateCommandBuffers(Context->GetDevice()->GetLogicalDevice(), &AllocInfo, &CommandBuffer);
		JE_CORE_ASSERT(AllocateResult == VK_SUCCESS, "Failed to allocate copy command buffer");
		
		VkCommandBufferBeginInfo BeginInfo{};
		BeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		BeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		const VkResult BeginResult = vkBeginCommandBuffer(CommandBuffer, &BeginInfo);
		JE_CORE_ASSERT(BeginResult == VK_SUCCESS, "Failed to begin command buffer");

		VkBufferCopy CopyRegion{};
		CopyRegion.size = Size;
		vkCmdCopyBuffer(CommandBuffer, SrcBuffer, DstBuffer, 1, &CopyRegion);

		const VkResult EndResult = vkEndCommandBuffer(CommandBuffer);
		JE_CORE_ASSERT(EndResult == VK_SUCCESS, "Failed to end command buffer");
		
		VkSubmitInfo SubmitInfo{};
		SubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		SubmitInfo.commandBufferCount = 1;
		SubmitInfo.pCommandBuffers = &CommandBuffer;

		const VkResult SubmitResult = vkQueueSubmit(Context->GetDevice()->GetGraphicsQueue(), 1, &SubmitInfo, VK_NULL_HANDLE);
		JE_CORE_ASSERT(SubmitResult == VK_SUCCESS, "Failed to submit copy command buffer");
		
		vkQueueWaitIdle(Context->GetDevice()->GetGraphicsQueue());

		vkFreeCommandBuffers(Context->GetDevice()->GetLogicalDevice(), Context->GetCommandPool(), 1, &CommandBuffer);
	}

	VulkanVertexBuffer::VulkanVertexBuffer(const std::vector<Vertex>& Vertexes)
	{
		const auto* Context = dynamic_cast<VulkanContext*>(VulkanContext::Get());

		const auto VertexCount = Vertexes.size();
		JE_CORE_ASSERT(VertexCount > 0, "Vertex buffer creation requires at least one vertex");
		
		const VkDeviceSize BufferSize = VertexCount * sizeof(Vertex);

		VkBuffer StagingBuffer;
		VkDeviceMemory StagingBufferMemory;
		CreateBuffer(BufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, StagingBuffer, StagingBufferMemory);

		void* Data;
		const VkResult MapResult = vkMapMemory(Context->GetDevice()->GetLogicalDevice(), StagingBufferMemory, 0, BufferSize, 0, &Data);
		JE_CORE_ASSERT(MapResult == VK_SUCCESS, "Failed to map staging buffer memory");
		
		memcpy(Data, Vertexes.data(), (size_t) BufferSize);
		vkUnmapMemory(Context->GetDevice()->GetLogicalDevice(), StagingBufferMemory);

		CreateBuffer(BufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, Buffer, BufferMemory);

		CopyBuffer(StagingBuffer, Buffer, BufferSize);

		vkDestroyBuffer(Context->GetDevice()->GetLogicalDevice(), StagingBuffer, nullptr);
		vkFreeMemory(Context->GetDevice()->GetLogicalDevice(), StagingBufferMemory, nullptr);
	}
	void* VulkanVertexBuffer::GetNativeHandle()
	{
		return  &Buffer;
	}

	VulkanVertexBuffer::~VulkanVertexBuffer() {}
	
	VulkanIndexBuffer::VulkanIndexBuffer(const std::vector<uint16_t>& Indexes)
	{
		const auto* Context = dynamic_cast<VulkanContext*>(VulkanContext::Get());
		JE_CORE_ASSERT(!Indexes.empty(), "Index buffer creation requires at least one index");
		VkDeviceSize BufferSize = sizeof(Indexes[0]) * Indexes.size();

		VkBuffer StagingBuffer;
		VkDeviceMemory StagingBufferMemory;
		CreateBuffer(BufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, StagingBuffer, StagingBufferMemory);

		void* Data;
		const VkResult MapResult = vkMapMemory(Context->GetDevice()->GetLogicalDevice(), StagingBufferMemory, 0, BufferSize, 0, &Data);
		JE_CORE_ASSERT(MapResult == VK_SUCCESS, "Failed to map staging buffer memory");
		
		memcpy(Data, Indexes.data(), (size_t) BufferSize);
		vkUnmapMemory(Context->GetDevice()->GetLogicalDevice(), StagingBufferMemory);

		CreateBuffer(BufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, Buffer, BufferMemory);

		CopyBuffer(StagingBuffer, Buffer, BufferSize);

		vkDestroyBuffer(Context->GetDevice()->GetLogicalDevice(), StagingBuffer, nullptr);
		vkFreeMemory(Context->GetDevice()->GetLogicalDevice(), StagingBufferMemory, nullptr);
	}
	void* VulkanIndexBuffer::GetNativeHandle()
	{
		return &Buffer;
	}

	VulkanIndexBuffer::~VulkanIndexBuffer() {}
} // namespace JuicyEngine