#pragma once

#include <sstream>
#include "JuicyEngine/Core.h"

namespace JuicyEngine
{
struct WindowProps
{
    std::string Title;
    uint32_t Width;
    uint32_t Height;
    WindowProps(const std::string& title = "JuicyEngine", uint32_t width = 1280, uint32_t height = 720)
        : Title(title), Width(width), Height(height)
    {
    }
};
// Interface representing a desktop system based Window
class Window
{
public:
    virtual ~Window() {}
    virtual uint32_t GetWidth() const = 0;
    virtual uint32_t GetHeight() const = 0;
    virtual void* GetNativeWindow() const = 0;
    static Scope<Window> Create(const WindowProps& props = WindowProps());
};
}  // namespace JuicyEngine