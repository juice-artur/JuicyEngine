#include "jepch.h"
#include "JuicyEngine/Renderer/VertexArray.h"
#include "JuicyEngine/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"
namespace JuicyEngine
{
Ref<VertexArray> VertexArray::Create()
{
    switch (Renderer::GetAPI())
    {
        case RendererAPI::API::None: JE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
        case RendererAPI::API::OpenGL: return CreateRef<OpenGLVertexArray>();
    }
    JE_CORE_ASSERT(false, "Unknown RendererAPI!");
    return nullptr;
}
}  // namespace JuicyEngine