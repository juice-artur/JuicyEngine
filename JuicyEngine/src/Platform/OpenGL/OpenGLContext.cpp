#include "jepch.h"
#include "Platform/OpenGL/OpenGLContext.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <GL/GL.h>
namespace JuicyEngine
{
OpenGLContext::OpenGLContext(GLFWwindow* windowHandle) : m_WindowHandle(windowHandle)
{
    JE_CORE_ASSERT(windowHandle, "Window handle is null!")
}
void OpenGLContext::Init()
{
    glfwMakeContextCurrent(m_WindowHandle);
    int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    JE_CORE_ASSERT(status, "Failed to initialize Glad!");

    JE_CORE_INFO("OpenGL Info:");
    JE_CORE_INFO("  Vendor: {0}", (const char*)glGetString(GL_VENDOR));
    JE_CORE_INFO("  Renderer: {0}", (const char*)glGetString(GL_RENDERER));
    JE_CORE_INFO("  Version: {0}", (const char*)glGetString(GL_VERSION));
}
void OpenGLContext::SwapBuffers()
{
    glfwSwapBuffers(m_WindowHandle);
}
}  // namespace JuicyEngine