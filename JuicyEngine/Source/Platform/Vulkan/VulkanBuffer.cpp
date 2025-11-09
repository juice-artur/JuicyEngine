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
	}
	
	void VulkanBuffer::UploadData(const void* Data, VkDeviceSize Size)
	{
		const auto* Context = dynamic_cast<VulkanContext*>(VulkanContext::Get());
		
		void* Mapped;
		vkMapMemory(Context->GetDevice()->GetLogicalDevice(), BufferMemory, 0, Size, 0, &Mapped);
		std::memcpy(Mapped, Data, static_cast<size_t>(Size));
		vkUnmapMemory(Context->GetDevice()->GetLogicalDevice(), BufferMemory);
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

	VulkanVertexBuffer::VulkanVertexBuffer(std::vector<Vertex> Vertexes)
	{
		VkBufferCreateInfo BufferInfo { .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
			                            .pNext = nullptr,
			                            .flags = 0,
			                            .size = Vertexes.size() * sizeof(Vertex),
			                            .usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			                            .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
			                            .queueFamilyIndexCount = 0,
			                            .pQueueFamilyIndices = nullptr };

		const auto* Context = dynamic_cast<VulkanContext*>(VulkanContext::Get());
		const auto Result = vkCreateBuffer(Context->GetDevice()->GetLogicalDevice(), &BufferInfo, nullptr, &Buffer);

		JE_CORE_ASSERT(Result == VK_SUCCESS, "Failed to create Vulkan buffer!")

		VkMemoryRequirements MemRequirements;
		vkGetBufferMemoryRequirements(Context->GetDevice()->GetLogicalDevice(), Buffer, &MemRequirements);

		VkMemoryAllocateInfo AllocInfo { .sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
			                             .pNext = nullptr,
			                             .allocationSize = MemRequirements.size,
			                             .memoryTypeIndex
			                             = FindMemoryType(MemRequirements.memoryTypeBits,
			                                              VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
			                                                  | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) };

		const auto AllocResult
		    = vkAllocateMemory(Context->GetDevice()->GetLogicalDevice(), &AllocInfo, nullptr, &BufferMemory);

		JE_CORE_ASSERT(AllocResult == VK_SUCCESS, "Failed to allocate vertex buffer memory")

		vkBindBufferMemory(Context->GetDevice()->GetLogicalDevice(), Buffer, BufferMemory, 0);

		UploadData(Vertexes.data(), Vertexes.size() * sizeof(Vertex));
	}

	VulkanVertexBuffer::~VulkanVertexBuffer()
	{
	}
} // namespace JuicyEngine