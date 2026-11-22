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

struct UniformBufferObject
{
    glm::mat4 Model;
    glm::mat4 View;
    glm::mat4 Proj;
};

struct TextureData
{
    std::vector<uint8_t> Pixels;
    int32_t Width = 0;
    int32_t Height = 0;

    bool Valid() const
    {
        return Width > 0 && Height > 0 &&
               Pixels.size() == static_cast<size_t>(Width) * static_cast<size_t>(Height) * 4;
    }
};

struct Mesh
{
    std::string Name;
    glm::mat4 Transform = glm::mat4(1.0f); 
    std::vector<Vertex> Vertices;
    std::vector<uint32_t> Indices;
    TextureData BaseColorTexture;
};

} // namespace JuicyEngine