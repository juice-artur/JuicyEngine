#include "jepch.h"
#include "WindowsWindow.h"
#include "JuicyEngine/Events/ApplicationEvent.h"
#include "JuicyEngine/Events/MouseEvent.h"
#include "JuicyEngine/Events/KeyEvent.h"

namespace JuicyEngine
{

LRESULT WindowsWindow::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    auto* data = reinterpret_cast<WindowsWindow::WindowData*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    if (!data || !data->EventCallback)
    {
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    switch (uMsg)
    {
        case WM_SIZE:
        {
            int width = LOWORD(lParam);
            int height = HIWORD(lParam);
            data->Width = width;
            data->Height = height;

            WindowResizeEvent event(width, height);
            data->EventCallback(event);
            break;
        }
        case WM_CLOSE:
        {
            WindowCloseEvent event;
            data->EventCallback(event);
            break;
        }
        case WM_KEYDOWN:
        {
            KeyPressedEvent event(static_cast<int>(wParam), 0);
            data->EventCallback(event);
            break;
        }
        case WM_KEYUP:
        {
            KeyReleasedEvent event(static_cast<int>(wParam));
            data->EventCallback(event);
            break;
        }
        case WM_MOUSEMOVE:
        {
            float xPos = static_cast<float>(LOWORD(lParam));
            float yPos = static_cast<float>(HIWORD(lParam));

            MouseMovedEvent event(xPos, yPos);
            data->EventCallback(event);
            break;
        }
        case WM_LBUTTONDOWN:
        {
            MouseButtonPressedEvent event(VK_LBUTTON);
            data->EventCallback(event);
            break;
        }
        case WM_LBUTTONUP:
        {
            MouseButtonReleasedEvent event(VK_LBUTTON);
            data->EventCallback(event);
            break;
        }
        case WM_MOUSEWHEEL:
        {
            float yOffset = static_cast<float>(GET_WHEEL_DELTA_WPARAM(wParam)) / WHEEL_DELTA;
            MouseScrolledEvent event(0.0f, yOffset);
            data->EventCallback(event);
            break;
        }
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

Scope<Window> Window::Create(const WindowProps& props)
{
    return CreateScope<WindowsWindow>(props);
}

WindowsWindow::WindowsWindow(const WindowProps& props)
{
    Init(props);
}

WindowsWindow::~WindowsWindow()
{
    Shutdown();
}

void WindowsWindow::Init(const WindowProps& props)
{
    m_Data.Title = props.Title;
    m_Data.Width = props.Width;
    m_Data.Height = props.Height;

    JE_CORE_INFO("Creating window {0} ({1}, {2})", props.Title, props.Width, props.Height);

    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpszClassName = "JuicyEngineWindow";

    RegisterClass(&wc);

    m_Window = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, "JuicyEngineWindow", TEXT(m_Data.Title.c_str()), WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, m_Data.Width, m_Data.Height, nullptr, nullptr, GetModuleHandle(nullptr), nullptr);

    if (!m_Window)
    {
        JE_CORE_CRITICAL("Failed to create WinAPI window!");
        return;
    }

    SetWindowLongPtr(m_Window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(&m_Data));

    ShowWindow(m_Window, SW_SHOW);
}

void WindowsWindow::Shutdown()
{
    DestroyWindow(m_Window);
}

void WindowsWindow::OnUpdate()
{
    MSG msg = {};
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}
};  // namespace JuicyEngine
