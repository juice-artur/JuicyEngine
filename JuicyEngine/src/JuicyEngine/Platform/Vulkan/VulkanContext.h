#pragma once

#include "JuicyEngine/Renderer/GraphicsContext.h"
#include "JuicyEngine/Core/Window.h"

namespace JuicyEngine
{
class VulkanContext : public GraphicsContext
{
public:
    VulkanContext(Window* windowHandle);

    // Inherited via GraphicsContext
    void Init() override;
    void SwapBuffers() override;

private:
    Window* m_Window;
};
}  // namespace JuicyEngine