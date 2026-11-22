#pragma once
#include "Mesh.h"

namespace JuicyEngine
{
class VertexBuffer
{
public:
    virtual ~VertexBuffer() = default;
};

class IndexBuffer
{
public:
    virtual ~IndexBuffer() = default;
};

class UniformBuffer
{
public:
    virtual ~UniformBuffer() = default;
    virtual void UploadData(int Size, void* Data) = 0;
};
} // namespace JuicyEngine
