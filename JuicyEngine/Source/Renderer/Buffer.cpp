#include "Buffer.h"

#include "Platform/Vulkan/VulkanBuffer.h"

namespace JuicyEngine
{
	VertexBuffer* VertexBuffer::Create(std::vector<Vertex> Vertexes)
	{
		return new VulkanVertexBuffer(Vertexes);
	}
	
	IndexBuffer* IndexBuffer::Create(std::vector<uint16_t> Indices)
	{
		return new VulkanIndexBuffer(Indices);
	}
} // namespace JuicyEngine
