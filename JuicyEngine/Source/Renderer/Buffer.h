#pragma once
#include "Mesh.h"

namespace JuicyEngine
{
	class VertexBuffer
	{
	public:
		virtual ~VertexBuffer() = default;
		virtual void* GetNativeHandle() = 0;
		
		static VertexBuffer* Create(std::vector<Vertex> Vertex);
	};

	class IndexBuffer
	{
	public:
		virtual ~IndexBuffer() = default;
		virtual void* GetNativeHandle() = 0;
		
		static IndexBuffer* Create(std::vector<uint16_t> Indices);
	};
} // namespace JuicyEngine