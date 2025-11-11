#pragma once

#include "Renderer/Buffer.h"
#include "vulkan/vulkan.h"

namespace JuicyEngine
{
	class VulkanBuffer
	{
	public:
		uint32_t FindMemoryType(uint32_t TypeFilter, VkMemoryPropertyFlags Properties);
		VkBuffer& GetBuffer();
		virtual ~VulkanBuffer();

	protected:
		void CreateBuffer(VkDeviceSize Size, VkBufferUsageFlags Usage, VkMemoryPropertyFlags Properties, VkBuffer& InBuffer, VkDeviceMemory& InBufferMemory);
		void CopyBuffer(VkBuffer SrcBuffer, VkBuffer DstBuffer, VkDeviceSize Size);
		
	protected:
		VkBuffer Buffer = VK_NULL_HANDLE;
		VkDeviceMemory BufferMemory = VK_NULL_HANDLE;
	};

	class VulkanVertexBuffer : public VertexBuffer, public VulkanBuffer
	{
	public:
		VulkanVertexBuffer(const std::vector<Vertex>& Vertexes);
		~VulkanVertexBuffer() override;
	};

	class VulkanIndexBuffer : public IndexBuffer, public VulkanBuffer
	{
	public:
		VulkanIndexBuffer(const std::vector<uint16_t>& Indexes);
		~VulkanIndexBuffer() override;
	};

	class VulkanUniformBuffer : public UniformBuffer, public VulkanBuffer
	{
	public:
		VulkanUniformBuffer(int Size);
		~VulkanUniformBuffer() override;
		void UploadData(int Size, void* Data) override;
	}; 
} // namespace JuicyEngine