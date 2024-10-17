#include "jepch.h"
#include "VertexArray.h"
#include "Renderer.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"
namespace JuicyEngine
{
VertexArray* VertexArray::Create()
{
    switch (Renderer::GetAPI())
    {
        case RendererAPI::None: JE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
        case RendererAPI::OpenGL: return new OpenGLVertexArray();
    }
    JE_CORE_ASSERT(false, "Unknown RendererAPI!");
    return nullptr;
}
}  // namespace JuicyEngine