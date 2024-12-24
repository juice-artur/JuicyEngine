#include "jepch.h"
#include "UniformBuffer.h"
#include "JuicyEngine/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLUniformBuffer.h"
namespace JuicyEngine
{
Ref<UniformBuffer> UniformBuffer::Create(uint32_t size, uint32_t binding)
{
    switch (Renderer::GetAPI())
    {
        case RendererAPI::API::None: JE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
        case RendererAPI::API::OpenGL: return CreateRef<OpenGLUniformBuffer>(size, binding);
    }
    JE_CORE_ASSERT(false, "Unknown RendererAPI!");
    return nullptr;
}
}  // namespace JuicyEngine