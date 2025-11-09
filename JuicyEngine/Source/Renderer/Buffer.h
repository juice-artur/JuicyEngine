#pragma once
#include "Mesh.h"

namespace JuicyEngine
{
	class VertexBuffer
	{
	public:
		virtual ~VertexBuffer() {}
		
		static VertexBuffer* Create(std::vector<Vertex> Vertex);
	};
}