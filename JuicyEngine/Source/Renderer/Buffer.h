#pragma once
#include "Mesh.h"

namespace JuicyEngine
{
	class VertexBuffer
	{
	public:
		virtual ~VertexBuffer() = default;
		static VertexBuffer* Create(std::vector<Vertex> Vertex);
	};

	class IndexBuffer
	{
	public:
		virtual ~IndexBuffer() = default;
		static IndexBuffer* Create(std::vector<uint16_t> Indices);
	};
} // namespace JuicyEngine