#include "jepch.h"
#include "WindowsWindow.h"
#include "JuicyEngine/Events/ApplicationEvent.h"
#include "JuicyEngine/Events/MouseEvent.h"
#include "JuicyEngine/Events/KeyEvent.h"
#include "JuicyEngine/RHI/VulkanRHI/VulkanContext.h"

namespace JuicyEngine
{

Window* Window::Create(const WindowProps& props)
{
    return new WindowsWindow(props);
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

    // Register class
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.lpfnWndProc = &WindowsWindow::WindowProc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.lpszClassName = "JuicyEngineWindowClass";
    RegisterClassEx(&wc);

    // Create window
    m_Window = CreateWindowEx(0, "JuicyEngineWindowClass", m_Data.Title.c_str(), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
        m_Data.Width, m_Data.Height, nullptr, nullptr, wc.hInstance, nullptr);

    if (!m_Window)
    {
        JE_CORE_ERROR("Failed to create window!");
        return;
    }

    ShowWindow(m_Window, SW_SHOW);
    SetVSync(true);

    // Set up the window's user pointer
    SetWindowLongPtr(m_Window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

    m_Context = new VulkanContext(this);
    m_Context->Init();
}

void WindowsWindow::Shutdown()
{
    DestroyWindow(m_Window);
}

void WindowsWindow::OnUpdate()
{
    ProcessMessages();
}

void WindowsWindow::ProcessMessages()
{
    MSG msg;
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);

        // If the message is WM_QUIT, the application should terminate
        if (msg.message == WM_QUIT)
        {
            // If WM_QUIT is received, exit the loop
            PostQuitMessage(0);
        }
    }
}

LRESULT CALLBACK WindowsWindow::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    WindowsWindow* window = reinterpret_cast<WindowsWindow*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    if (!window)
    {
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    WindowData& data = window->m_Data;

    switch (uMsg)
    {
        case WM_CLOSE:
        {
            WindowCloseEvent event;
            data.EventCallback(event);
            return 0;
        }
        case WM_SIZE:
        {
            WindowResizeEvent event(LOWORD(lParam), HIWORD(lParam));
            data.EventCallback(event);
            return 0;
        }
        case WM_KEYDOWN:
        {
            int key = static_cast<int>(wParam);
            KeyPressedEvent event(key, 0);
            data.EventCallback(event);
            return 0;
        }
        case WM_KEYUP:
        {
            int key = static_cast<int>(wParam);
            KeyReleasedEvent event(key);
            data.EventCallback(event);
            return 0;
        }
        case WM_LBUTTONDOWN:
        {
            int button = 0;  // Left button
            MouseButtonPressedEvent event(button);
            data.EventCallback(event);
            return 0;
        }
        case WM_LBUTTONUP:
        {
            int button = 0;  // Left button
            MouseButtonReleasedEvent event(button);
            data.EventCallback(event);
            return 0;
        }
        case WM_MOUSEMOVE:
        {
            POINTS pt = MAKEPOINTS(lParam);
            MouseMovedEvent event(pt.x, pt.y);
            data.EventCallback(event);
            return 0;
        }
        case WM_MOUSEWHEEL:
        {
            int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
            MouseScrolledEvent event(0, zDelta);
            data.EventCallback(event);
            return 0;
        }
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void WindowsWindow::SetVSync(bool enabled)
{
    // This is a dummy implementation, as WinAPI does not have direct control over V-Sync
    m_Data.VSync = enabled;
}

bool WindowsWindow::IsVSync() const
{
    return m_Data.VSync;
}
};  // namespace JuicyEngine
