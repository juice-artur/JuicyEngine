#include "jepch.h"
#include "JuicyEngine/Renderer/RenderCommand.h"
namespace JuicyEngine
{
Scope<RendererAPI> RenderCommand::s_RendererAPI = RendererAPI::Create();
}