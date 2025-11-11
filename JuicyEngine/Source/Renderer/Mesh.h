#pragma once

#include <glm/glm.hpp>

namespace JuicyEngine
{
	struct Vertex
	{
		glm::vec2 Position;
		glm::vec3 Color;
	};

	struct UniformBufferObject {
		glm::mat4 Model;
		glm::mat4 View;
		glm::mat4 Proj;
	};
} // namespace JuicyEngine