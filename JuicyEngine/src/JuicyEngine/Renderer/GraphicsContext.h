#pragma once

namespace JuicyEngine
{
class GraphicsContext
{
public:
    virtual void Init() = 0;
    virtual void SwapBuffers() = 0;
    virtual void DrawFrame() = 0;
    virtual void RecreateSwapchain() = 0;
    virtual ~GraphicsContext() {};
};
}  // namespace JuicyEngine