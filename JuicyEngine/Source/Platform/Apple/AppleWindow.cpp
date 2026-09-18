#include "AppleWindow.h"
#include "Core/Log.h"
#include "Events/ApplicationEvent.h"
#include "Events/KeyEvent.h"
#include "Events/MouseEvent.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_hints.h>

#include <backends/imgui_impl_sdl3.h>

namespace JuicyEngine
{
static bool s_SDLInitialized = false;

AppleWindow::AppleWindow(const WindowProps& props)
{
    Init(props);
}

AppleWindow::~AppleWindow()
{
    Shutdown();
}

void AppleWindow::Init(const WindowProps& props)
{
    m_Data.Title = props.Title;
    m_Data.Width = props.Width;
    m_Data.Height = props.Height;

    JE_CORE_INFO("Creating window {0} ({1}, {2})", props.Title, props.Width, props.Height);

    if (!s_SDLInitialized)
    {
        JE_CORE_ASSERT(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS), "Could not initialize SDL3: {0}", SDL_GetError())
        s_SDLInitialized = true;
    }

#if defined(__APPLE__)
    // On iOS touches are converted into mouse events, which keeps the engine
    // and ImGui input working through the standard mouse event pipeline.
    SDL_SetHint(SDL_HINT_TOUCH_MOUSE_EVENTS, "1");
#endif

    Uint32 flags = SDL_WINDOW_VULKAN;

#if !defined(__APPLE__)
    flags |= SDL_WINDOW_RESIZABLE;
#endif

    m_Window = SDL_CreateWindow(m_Data.Title.c_str(), m_Data.Width, m_Data.Height, flags);
    if (!m_Window)
    {
        JE_CORE_ASSERT(m_Window, "Failed to create SDL window: {0}", SDL_GetError())
    }

    SetVSync(true);
}

void AppleWindow::Shutdown()
{
    SDL_DestroyWindow(m_Window);
}

void AppleWindow::OnUpdate()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        ImGui_ImplSDL3_ProcessEvent(&event);

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
                MouseScrolledEvent e((float) event.wheel.x, (float) event.wheel.y);
                m_Data.EventCallback(e);
                break;
            }

            case SDL_EVENT_MOUSE_MOTION:
            {
                MouseMovedEvent e((float) event.motion.x, (float) event.motion.y);
                m_Data.EventCallback(e);
                break;
            }

        }
    }
}

void AppleWindow::SetVSync(bool enabled)
{
    m_Data.VSync = enabled;
}

bool AppleWindow::IsVSync() const
{
    return m_Data.VSync;
}

void* AppleWindow::GetNativeWindow() const
{
    return (void*) m_Window;
}
} // namespace JuicyEngine