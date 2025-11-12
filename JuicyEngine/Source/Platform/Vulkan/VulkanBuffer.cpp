#include "VulkanBuffer.h"

#include "VulkanContext.h"
#include "VulkanUtils.h"
#include "Core/Core.h"

namespace JuicyEngine
{
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
	
	void VulkanBuffer::CopyBuffer(VkBuffer SrcBuffer, VkBuffer DstBuffer, VkDeviceSize Size)
	{
		VkCommandBuffer CommandBuffer = RHI::Vulkan::VulkanUtils::BeginSingleTimeCommands();

		VkBufferCopy CopyRegion{};
		CopyRegion.size = Size;
		vkCmdCopyBuffer(CommandBuffer, SrcBuffer, DstBuffer, 1, &CopyRegion);

		RHI::Vulkan::VulkanUtils::EndSingleTimeCommands(CommandBuffer);
	}

	VulkanVertexBuffer::VulkanVertexBuffer(const std::vector<Vertex>& Vertexes)
	{
		const auto* Context = dynamic_cast<VulkanContext*>(VulkanContext::Get());

		const auto VertexCount = Vertexes.size();
		JE_CORE_ASSERT(VertexCount > 0, "Vertex buffer creation requires at least one vertex")
		
		const VkDeviceSize BufferSize = VertexCount * sizeof(Vertex);

		VkBuffer StagingBuffer;
		VkDeviceMemory StagingBufferMemory;
		RHI::Vulkan::VulkanUtils::CreateBuffer(BufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, StagingBuffer, StagingBufferMemory);

		void* Data;
		const VkResult MapResult = vkMapMemory(Context->GetDevice()->GetLogicalDevice(), StagingBufferMemory, 0, BufferSize, 0, &Data);
		JE_CORE_ASSERT(MapResult == VK_SUCCESS, "Failed to map staging buffer memory")
		
		memcpy(Data, Vertexes.data(), (size_t) BufferSize);
		vkUnmapMemory(Context->GetDevice()->GetLogicalDevice(), StagingBufferMemory);

		RHI::Vulkan::VulkanUtils::CreateBuffer(BufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, Buffer, BufferMemory);

		CopyBuffer(StagingBuffer, Buffer, BufferSize);

		vkDestroyBuffer(Context->GetDevice()->GetLogicalDevice(), StagingBuffer, nullptr);
		vkFreeMemory(Context->GetDevice()->GetLogicalDevice(), StagingBufferMemory, nullptr);
	}

	VulkanVertexBuffer::~VulkanVertexBuffer() {}
	
	VulkanIndexBuffer::VulkanIndexBuffer(const std::vector<uint16_t>& Indexes)
	{
		const auto* Context = dynamic_cast<VulkanContext*>(VulkanContext::Get());
		JE_CORE_ASSERT(!Indexes.empty(), "Index buffer creation requires at least one index")
		VkDeviceSize BufferSize = sizeof(Indexes[0]) * Indexes.size();

		VkBuffer StagingBuffer;
		VkDeviceMemory StagingBufferMemory;
		RHI::Vulkan::VulkanUtils::CreateBuffer(BufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, StagingBuffer, StagingBufferMemory);

		void* Data;
		const VkResult MapResult = vkMapMemory(Context->GetDevice()->GetLogicalDevice(), StagingBufferMemory, 0, BufferSize, 0, &Data);
		JE_CORE_ASSERT(MapResult == VK_SUCCESS, "Failed to map staging buffer memory")
		
		memcpy(Data, Indexes.data(), (size_t) BufferSize);
		vkUnmapMemory(Context->GetDevice()->GetLogicalDevice(), StagingBufferMemory);

		RHI::Vulkan::VulkanUtils::CreateBuffer(BufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, Buffer, BufferMemory);

		CopyBuffer(StagingBuffer, Buffer, BufferSize);

		vkDestroyBuffer(Context->GetDevice()->GetLogicalDevice(), StagingBuffer, nullptr);
		vkFreeMemory(Context->GetDevice()->GetLogicalDevice(), StagingBufferMemory, nullptr);
	}

	VulkanIndexBuffer::~VulkanIndexBuffer() {}
	
	VulkanUniformBuffer::VulkanUniformBuffer(int Size)
	{
		RHI::Vulkan::VulkanUtils::CreateBuffer(Size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, Buffer, BufferMemory);
	}
	
	VulkanUniformBuffer::~VulkanUniformBuffer() {}
	
	void VulkanUniformBuffer::UploadData(int Size, void* Data)
	{
		const auto* Context = dynamic_cast<VulkanContext*>(VulkanContext::Get());
    
		void* MappedData = nullptr;
		VkResult MapResult = vkMapMemory(Context->GetDevice()->GetLogicalDevice(), BufferMemory, 0, Size, 0, &MappedData);
		JE_CORE_ASSERT(MapResult == VK_SUCCESS, "Failed to map uniform buffer memory")

		memcpy(MappedData, Data, Size);
		vkUnmapMemory(Context->GetDevice()->GetLogicalDevice(), BufferMemory);
	}
} // namespace JuicyEngine