#include "jepch.h"
#include "Platform/OpenGL/OpenGLTexture.h"
#include "JuicyEngine/Renderer/Texture.h"
#include "JuicyEngine/Renderer/Renderer.h"
namespace JuicyEngine
{

Ref<Texture2D> Texture2D::Create(uint32_t width, uint32_t height)
{
    switch (Renderer::GetAPI())
    {
        case RendererAPI::API::None: JE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
        case RendererAPI::API::OpenGL: return CreateRef<OpenGLTexture2D>(width, height);
    }
    JE_CORE_ASSERT(false, "Unknown RendererAPI!");
    return nullptr;
}
Ref<Texture2D> Texture2D::Create(const std::string& path)
{
    switch (Renderer::GetAPI())
    {
        case RendererAPI::API::None: JE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
        case RendererAPI::API::OpenGL: return CreateRef<OpenGLTexture2D>(path);
    }
    JE_CORE_ASSERT(false, "Unknown RendererAPI!");
    return nullptr;
}
}  // namespace JuicyEngine