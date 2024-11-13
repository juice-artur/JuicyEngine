#include "jepch.h"
#include "JuicyEngine/Core/Window.h"
#ifdef JE_PLATFORM_WINDOWS
#include "Platform/Windows/WindowsWindow.h"
#endif
namespace JuicyEngine
{
Scope<Window> Window::Create(const WindowProps& props)
{
#ifdef JE_PLATFORM_WINDOWS
    return CreateScope<WindowsWindow>(props);
#else
    JE_CORE_ASSERT(false, "Unknown platform!");
    return nullptr;
#endif
}
}  // namespace JuicyEngine