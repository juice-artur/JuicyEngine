#include "jepch.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"
#include <glad/glad.h>
namespace JuicyEngine
{
static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
{
    switch (type)
    {
        case JuicyEngine::ShaderDataType::Float: return GL_FLOAT;
        case JuicyEngine::ShaderDataType::Float2: return GL_FLOAT;
        case JuicyEngine::ShaderDataType::Float3: return GL_FLOAT;
        case JuicyEngine::ShaderDataType::Float4: return GL_FLOAT;
        case JuicyEngine::ShaderDataType::Mat3: return GL_FLOAT;
        case JuicyEngine::ShaderDataType::Mat4: return GL_FLOAT;
        case JuicyEngine::ShaderDataType::Int: return GL_INT;
        case JuicyEngine::ShaderDataType::Int2: return GL_INT;
        case JuicyEngine::ShaderDataType::Int3: return GL_INT;
        case JuicyEngine::ShaderDataType::Int4: return GL_INT;
        case JuicyEngine::ShaderDataType::Bool: return GL_BOOL;
    }
    JE_CORE_ASSERT(false, "Unknown ShaderDataType!");
    return 0;
}
OpenGLVertexArray::OpenGLVertexArray()
{
    glCreateVertexArrays(1, &m_RendererID);
}
OpenGLVertexArray::~OpenGLVertexArray()
{
    glDeleteVertexArrays(1, &m_RendererID);
}
void OpenGLVertexArray::Bind() const
{
    glBindVertexArray(m_RendererID);
}
void OpenGLVertexArray::Unbind() const
{
    glBindVertexArray(0);
}
void OpenGLVertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer)
{
    JE_CORE_ASSERT(vertexBuffer->GetLayout().GetElements().size(), "Vertex Buffer has no layout!");
    glBindVertexArray(m_RendererID);
    vertexBuffer->Bind();
    uint32_t index = 0;
    const auto& layout = vertexBuffer->GetLayout();
    for (const auto& element : layout)
    {
        // glEnableVertexAttribArray(index + m_VertexBufferIndexOffset);
        // glVertexAttribPointer(index + m_VertexBufferIndexOffset, element.GetComponentCount(),
        // ShaderDataTypeToOpenGLBaseType(element.Type),
        //     element.Normalized ? GL_TRUE : GL_FALSE, layout.GetStride(), (const void*)element.Offset);
        // index++;

        switch (element.Type)
        {
            case ShaderDataType::Float:
            case ShaderDataType::Float2:
            case ShaderDataType::Float3:
            case ShaderDataType::Float4:
            case ShaderDataType::Int:
            case ShaderDataType::Int2:
            case ShaderDataType::Int3:
            case ShaderDataType::Int4:
            case ShaderDataType::Bool:
            {
                glEnableVertexAttribArray(m_VertexBufferIndexOffset);
                glVertexAttribPointer(m_VertexBufferIndexOffset, element.GetComponentCount(), ShaderDataTypeToOpenGLBaseType(element.Type),
                    element.Normalized ? GL_TRUE : GL_FALSE, layout.GetStride(), (const void*)element.Offset);
                m_VertexBufferIndexOffset++;
                break;
            }
            case ShaderDataType::Mat3:
            case ShaderDataType::Mat4:
            {
                uint8_t count = element.GetComponentCount();
                for (uint8_t i = 0; i < count; i++)
                {
                    glEnableVertexAttribArray(m_VertexBufferIndexOffset);
                    glVertexAttribPointer(m_VertexBufferIndexOffset, count, ShaderDataTypeToOpenGLBaseType(element.Type),
                        element.Normalized ? GL_TRUE : GL_FALSE, layout.GetStride(),
                        (const void*)(element.Offset + sizeof(float) * count * i));
                    glVertexAttribDivisor(m_VertexBufferIndexOffset, 1);
                    m_VertexBufferIndexOffset++;
                }
                break;
            }
            default: JE_CORE_ASSERT(false, "Unknown ShaderDataType!");
        }
    }
    m_VertexBuffers.push_back(vertexBuffer);
    m_VertexBufferIndexOffset += layout.GetElements().size();
}
void OpenGLVertexArray::SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer)
{
    glBindVertexArray(m_RendererID);
    indexBuffer->Bind();
    m_IndexBuffer = indexBuffer;
}
}  // namespace JuicyEngine