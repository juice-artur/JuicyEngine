#include "Buffer.h"

#include "Platform/Vulkan/VulkanBuffer.h"

namespace JuicyEngine
{
	std::unique_ptr<VertexBuffer> VertexBuffer::Create(std::vector<Vertex> Vertexes)
	{
		return std::make_unique<VulkanVertexBuffer>(Vertexes);
	}
} // namespace JuicyEngine
