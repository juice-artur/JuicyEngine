#pragma once

#include <glm/glm.hpp>

namespace JuicyEngine
{
	struct Vertex
	{
		glm::vec3 Position;
		glm::vec3 Color;
		glm::vec2 TexCoord;
	};

	struct UniformBufferObject {
		glm::mat4 Model;
		glm::mat4 View;
		glm::mat4 Proj;
	};
} // namespace JuicyEngine