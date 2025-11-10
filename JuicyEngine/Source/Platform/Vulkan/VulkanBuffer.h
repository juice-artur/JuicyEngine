#pragma once

#include "Renderer/Buffer.h"
#include "vulkan/vulkan.h"

namespace JuicyEngine
{
	class VulkanBuffer
	{
	public:
		uint32_t FindMemoryType(uint32_t TypeFilter, VkMemoryPropertyFlags Properties);
		void UploadData(const void* Data, VkDeviceSize Size);
		VkBuffer& GetBuffer();
		virtual ~VulkanBuffer();

	protected:
		VkBuffer Buffer;
		VkDeviceMemory BufferMemory;
	};

	class VulkanVertexBuffer : public VertexBuffer, public VulkanBuffer
	{
	public:
		VulkanVertexBuffer(const std::vector<Vertex>& Vertexes);

		~VulkanVertexBuffer() override;
	};
} // namespace JuicyEngine