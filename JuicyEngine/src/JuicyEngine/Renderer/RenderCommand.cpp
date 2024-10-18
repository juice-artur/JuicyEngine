#include "jepch.h"
#include "RenderCommand.h"
#include "Platform/OpenGL/OpenGLRendererAPI.h"
namespace JuicyEngine
{
RendererAPI* RenderCommand::s_RendererAPI = new OpenGLRendererAPI;
}