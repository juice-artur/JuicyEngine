#include "jepch.h"

#include "Core/Window.h"

#if defined(_WIN32)
#include "Platform/Windows/WindowsWindow.h"
#elif defined(__APPLE__)
#include "Platform/Apple/AppleWindow.h"
#endif

namespace JuicyEngine
{
std::unique_ptr<Window> Window::Create(const WindowProps& props)
{
#if defined(_WIN32)
    return std::make_unique<WindowsWindow>(props);
#elif defined(__APPLE__)
    return std::make_unique<AppleWindow>(props);
#else
    return std::make_unique<WindowsWindow>(props);
#endif
}
} // namespace JuicyEngine