#include "Buffer.h"

#include "Platform/Vulkan/VulkanBuffer.h"

namespace JuicyEngine
{
	VertexBuffer* VertexBuffer::Create(std::vector<Vertex> Vertexes)
	{
		return new VulkanVertexBuffer(Vertexes);
	}
}

