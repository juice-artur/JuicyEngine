#include "jepch.h"
#include "Shader.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLShader.h"

namespace JuicyEngine
{
Shader* Shader::Create(const std::string& filepath)
{
    switch (Renderer::GetAPI())
    {
        case RendererAPI::API::None: JE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
        case RendererAPI::API::OpenGL: return new OpenGLShader(filepath);
    }
    JE_CORE_ASSERT(false, "Unknown RendererAPI!");
    return nullptr;
}

Shader* Shader::Create(const std::string& vertexSrc, const std::string& fragmentSrc)
{
    switch (Renderer::GetAPI())
    {
        case RendererAPI::API::None: JE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
        case RendererAPI::API::OpenGL: return new OpenGLShader(vertexSrc, fragmentSrc);
    }

    JE_CORE_ASSERT(false, "Unknown RendererAPI!");
    return nullptr;
}

}  // namespace JuicyEngine