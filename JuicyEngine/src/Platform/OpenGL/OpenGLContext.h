#pragma once
#include "JuicyEngine/Renderer/GraphicsContext.h"

struct GLFWwindow;

namespace JuicyEngine
{
class OpenGLContext : public GraphicsContext
{
public:
    OpenGLContext(GLFWwindow* windowHandle);
    virtual void Init() override;
    virtual void SwapBuffers() override;

private:
    GLFWwindow* m_WindowHandle;
};

}  // namespace JuicyEngine