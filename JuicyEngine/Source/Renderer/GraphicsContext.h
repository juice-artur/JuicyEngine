#pragma once

#include <memory>

namespace JuicyEngine
{
enum class RendererAPI { Vulkan };

class GraphicsContext
{
public:
    virtual ~GraphicsContext() = default;

    static std::unique_ptr<GraphicsContext> Create(RendererAPI API = RendererAPI::Vulkan);

    virtual void Init(void* Window) = 0;
    virtual void SwapBuffers() = 0;
    virtual void Shutdown() = 0;
    virtual void Draw() = 0;
};
} // namespace JuicyEngine
