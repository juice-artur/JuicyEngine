#include "jepch.h"
#include "JuicyEngine/Renderer/Shader.h"

#include "JuicyEngine/Renderer/Renderer.h"
#include "Platform/OpenGL/OpenGLShader.h"

namespace JuicyEngine
{
Ref<Shader> Shader::Create(const std::string& filepath)
{
    switch (Renderer::GetAPI())
    {
        case RendererAPI::API::None: JE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
        case RendererAPI::API::OpenGL: return CreateRef<OpenGLShader>(filepath);
    }
    JE_CORE_ASSERT(false, "Unknown RendererAPI!");
    return nullptr;
}

Ref<Shader> Shader::Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
{
    switch (Renderer::GetAPI())
    {
        case RendererAPI::API::None: JE_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
        case RendererAPI::API::OpenGL: return CreateRef<OpenGLShader>(name, vertexSrc, fragmentSrc);
    }

    JE_CORE_ASSERT(false, "Unknown RendererAPI!");
    return nullptr;
}

void ShaderLibrary::Add(const std::string& name, const Ref<Shader>& shader)
{
    JE_CORE_ASSERT(!Exists(name), "Shader already exists!");
    m_Shaders[name] = shader;
}
void ShaderLibrary::Add(const Ref<Shader>& shader)
{
    auto& name = shader->GetName();
    Add(name, shader);
}
JuicyEngine::Ref<JuicyEngine::Shader> ShaderLibrary::Load(const std::string& filepath)
{
    auto shader = Shader::Create(filepath);
    Add(shader);
    return shader;
}
JuicyEngine::Ref<JuicyEngine::Shader> ShaderLibrary::Load(const std::string& name, const std::string& filepath)
{
    auto shader = Shader::Create(filepath);
    Add(name, shader);
    return shader;
}
JuicyEngine::Ref<JuicyEngine::Shader> ShaderLibrary::Get(const std::string& name)
{
    JE_CORE_ASSERT(Exists(name), "Shader not found!");
    return m_Shaders[name];
}
bool ShaderLibrary::Exists(const std::string& name) const
{
    return m_Shaders.find(name) != m_Shaders.end();
}

}  // namespace JuicyEngine