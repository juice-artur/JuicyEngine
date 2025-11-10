#pragma once
#include "Mesh.h"

namespace JuicyEngine
{
	class VertexBuffer
	{
	public:
		virtual ~VertexBuffer() {}

		static std::unique_ptr<VertexBuffer> Create(std::vector<Vertex> Vertex);
	};
} // namespace JuicyEngine