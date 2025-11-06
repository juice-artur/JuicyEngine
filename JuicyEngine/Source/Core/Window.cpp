#include "jepch.h"

#include "Core/Window.h"
#include "Platform/Windows/WindowsWindow.h"

namespace JuicyEngine
{
	std::unique_ptr<Window> Window::Create(const WindowProps& props)
	{
		return std::make_unique<WindowsWindow>(props);
	}
} // namespace JuicyEngine