#include "WindowsWindow.h"
#include <Core\Log.h>

WindowHandle_t WindowsWindow::GetHandle() const
{
	return handle;
}

WindowsWindow::~WindowsWindow()
{
	Close();
}

WindowsWindow::WindowsWindow() : bIsShowCursor(false), bIsFullscreen(false), sdlWindow(nullptr), handle(nullptr) {}

bool WindowsWindow::Create(const char* InTitle, unsigned int InWidth, unsigned int InHeight, unsigned int InFlags)
{
	if (sdlWindow)
	{
		LOG_WARN("Warning: CWindowsWindow::Create: Window already created");
		return true;
	}

	unsigned int sdlFlags = 0;
	if (InFlags & SW_Hidden)
	{
		sdlFlags = SDL_WINDOW_HIDDEN;
	}

	else
	{
		if (InFlags & SW_Resizable)
		{
			sdlFlags |= SDL_WINDOW_RESIZABLE;
		}

		if (!(InFlags & SW_Decorated))
		{
			sdlFlags |= SDL_WINDOW_BORDERLESS;
		}

		if (InFlags & SW_Minimized)
		{
			sdlFlags |= SDL_WINDOW_MINIMIZED;
		}

		if (InFlags & SW_Maximized)
		{
			sdlFlags |= SDL_WINDOW_MAXIMIZED;
		}
	}
	sdlWindow = SDL_CreateWindow(InTitle, InWidth, InHeight, sdlFlags);

	if (!sdlWindow)
	{
		LOG_ERROR("Error: Failed created window ({0}x{1}) with title '{2}' and flags {3}. SDL error: {4}", InWidth, InHeight, InTitle,
			InFlags, SDL_GetError());
		return false;
	}

	handle = (HWND)SDL_GetProperty(SDL_GetWindowProperties(sdlWindow), SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL);
	LOG_INFO("Window with handle {0} successfully created", handle);

	return true;
}

void WindowsWindow::Close()
{
	if (!sdlWindow)
	{
		return;
	}

	SDL_DestroyWindow(sdlWindow);

	if (!bIsShowCursor)
	{
		SDL_SetRelativeMouseMode(SDL_FALSE);
	}

	sdlWindow = nullptr;
	handle = nullptr;
	bIsFullscreen = false;
	bIsShowCursor = false;
	LOG_TRACE("Window with handle {0} closed", handle);
}

void WindowsWindow::ShowCursor(bool InIsShowCursor)
{
	bIsShowCursor = InIsShowCursor;
	SDL_SetRelativeMouseMode(InIsShowCursor ? SDL_FALSE : SDL_TRUE);
}

void WindowsWindow::ShowWindow(bool InIsShowWindow)
{
	if (sdlWindow)
	{
		if (InIsShowWindow)
		{
			SDL_ShowWindow(sdlWindow);
		}
		else
		{
			SDL_HideWindow(sdlWindow);
		}
	}
}

void WindowsWindow::Maximize()
{
	if (sdlWindow)
	{
		SDL_MaximizeWindow(sdlWindow);
	}
}

void WindowsWindow::Minimize()
{
	if (sdlWindow)
	{
		SDL_MinimizeWindow(sdlWindow);
	}
}

bool WindowsWindow::PollEvent(WindowEvent& OutWindowEvent) const
{
	if (!sdlWindow)
	{
		return false;
	}

	// By default set window event to none
	OutWindowEvent.type = WET_None;

	// Polls for currently pending SDL2 events
	SDL_Event sdlEvent;
	bool bIsNotEndQueue = SDL_PollEvent(&sdlEvent);

	// Handle SDL2 events
	switch (sdlEvent.type)
	{
		case SDL_EVENT_WINDOW_CLOSE_REQUESTED: OutWindowEvent.type = WET_WindowClose; break;
		default: OutWindowEvent.type = WET_None; break;
	}

	return bIsNotEndQueue;
}

void WindowsWindow::SetTitle(const char* InTitle)
{
	if (sdlWindow)
	{
		SDL_SetWindowTitle(sdlWindow, InTitle);
	}
}

void WindowsWindow::SetSize(unsigned int InWidth, unsigned int InHeight)
{
	if (sdlWindow)
	{
		SDL_SetWindowSize(sdlWindow, InWidth, InHeight);
		SDL_SetWindowPosition(sdlWindow, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
	}
}

void WindowsWindow::SetFullscreen(bool InIsFullscreen)
{
	if (sdlWindow)
	{
		bIsFullscreen = InIsFullscreen;
		SDL_SetWindowFullscreen(sdlWindow, InIsFullscreen ? SDL_WINDOW_FULLSCREEN : 0);
	}
}

bool WindowsWindow::IsOpen() const
{
	return sdlWindow;
}

bool WindowsWindow::IsShowingCursor() const
{
	return bIsShowCursor;
}

bool WindowsWindow::IsFullscreen() const
{
	return bIsFullscreen;
}

void WindowsWindow::GetSize(unsigned int& OutWidth, unsigned int& OutHeight) const
{
	if (sdlWindow)
	{
		SDL_GetWindowSize(sdlWindow, (int*)&OutWidth, (int*)&OutHeight);
	}
}