#include "WindowsWindow.h"
#include "Core/Log.h"
#include "Events/ApplicationEvent.h"
#include "Events/KeyEvent.h"
#include "Events/MouseEvent.h"
#include <SDL3/SDL.h>

#include "Platform/Vulkan/VulkanContext.h"

namespace JuicyEngine
{
static bool s_SDLInitialized = false;

WindowsWindow::WindowsWindow(const WindowProps& props)
{
    Init(props);
}

WindowsWindow::~WindowsWindow()
{
    m_Context->Shutdown();
}

void WindowsWindow::Init(const WindowProps& props)
{
    m_Data.Title = props.Title;
    m_Data.Width = props.Width;
    m_Data.Height = props.Height;

    JE_CORE_INFO("Creating window {0} ({1}, {2})", props.Title, props.Width, props.Height);

    if (!s_SDLInitialized)
    {
        JE_CORE_ASSERT(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS), "Could not initialize SDL3: {0}", SDL_GetError());
        s_SDLInitialized = true;
    }

    Uint32 flags = SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE;
    m_Window = SDL_CreateWindow(m_Data.Title.c_str(), m_Data.Width, m_Data.Height, flags);
    if (!m_Window)
    {
        JE_CORE_ASSERT(m_Window, "Failed to create SDL window: {0}", SDL_GetError());
    }

    m_Context = new VulkanContext();
    m_Context->Init(GetNativeWindow());

    SetVSync(true);
}

void WindowsWindow::Shutdown()
{
    m_Context->Shutdown();
    SDL_DestroyWindow(m_Window);
    SDL_Quit();
}

void WindowsWindow::OnUpdate()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_EVENT_QUIT:
            {
                WindowCloseEvent e;
                m_Data.EventCallback(e);
                break;
            }

            case SDL_EVENT_WINDOW_RESIZED:
            {
                m_Data.Width = event.window.data1;
                m_Data.Height = event.window.data2;
                WindowResizeEvent e(m_Data.Width, m_Data.Height);
                m_Data.EventCallback(e);
                break;
            }

            case SDL_EVENT_KEY_DOWN:
            {
                KeyPressedEvent e(event.key.key, event.key.repeat ? 1 : 0);
                m_Data.EventCallback(e);
                break;
            }

            case SDL_EVENT_KEY_UP:
            {
                KeyReleasedEvent e(event.key.key);
                m_Data.EventCallback(e);
                break;
            }

            case SDL_EVENT_MOUSE_BUTTON_DOWN:
            {
                MouseButtonPressedEvent e(event.button.button);
                m_Data.EventCallback(e);
                break;
            }

            case SDL_EVENT_MOUSE_BUTTON_UP:
            {
                MouseButtonReleasedEvent e(event.button.button);
                m_Data.EventCallback(e);
                break;
            }

            case SDL_EVENT_MOUSE_WHEEL:
            {
                MouseScrolledEvent e((float)event.wheel.x, (float)event.wheel.y);
                m_Data.EventCallback(e);
                break;
            }

            case SDL_EVENT_MOUSE_MOTION:
            {
                MouseMovedEvent e((float)event.motion.x, (float)event.motion.y);
                m_Data.EventCallback(e);
                break;
            }
        }
    }

    m_Context->SwapBuffers();
}

void WindowsWindow::SetVSync(bool enabled)
{
    m_Data.VSync = enabled;
}

bool WindowsWindow::IsVSync() const
{
    return m_Data.VSync;
}

void* WindowsWindow::GetNativeWindow() const
{
    void* hwnd = (void*)SDL_GetPointerProperty(SDL_GetWindowProperties(m_Window), SDL_PROP_WINDOW_WIN32_HWND_POINTER, NULL);
    return hwnd;
}
}