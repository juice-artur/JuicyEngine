#include "jepch.h"
#include "JuicyEngine/Renderer/Framebuffer.h"

#include "JuicyEngine/Renderer/Renderer.h"

#include "Platform/OpenGL/OpenGLFramebuffer.h"

namespace JuicyEngine
{

Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification& spec)
{
    switch (Renderer::GetAPI())
    {
        case RendererAPI::API::None:
            JE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
            {
                return nullptr;
            }
        case RendererAPI::API::OpenGL: return CreateRef<OpenGLFramebuffer>(spec);
    }

    JE_CORE_ASSERT(false, "Unknown RendererAPI!");
    return nullptr;
}

}  // namespace JuicyEngine
