#include "jepch.h"

#include "WindowsWindow.h"

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

    JE_CORE_INFO("Creating window {0} ({1}, {2})", props.Title, props.Width, props.Height);

    m_HInstance = GetModuleHandle(nullptr);

    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = m_HInstance;
    wc.lpszClassName = "JuicyEngineWindowClass";

    RegisterClass(&wc);

    m_WindowHandle = CreateWindowEx(0, wc.lpszClassName, m_Data.Title.c_str(), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
        m_Data.Width, m_Data.Height, nullptr, nullptr, m_HInstance, this);

    JE_CORE_ASSERT(m_WindowHandle, "Failed to create a window!");

    ShowWindow(m_WindowHandle, SW_SHOW);
    SetVSync(true);
}

void WindowsWindow::Shutdown()
{
    DestroyWindow(m_WindowHandle);
}

void WindowsWindow::OnUpdate()
{
    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

void WindowsWindow::SetVSync(bool enabled)
{
    // TODO: Implement VSync
    m_Data.VSync = enabled;
}

bool WindowsWindow::IsVSync() const
{
    return m_Data.VSync;
}

LRESULT CALLBACK WindowsWindow::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_CLOSE:
        case WM_DESTROY:
            PostQuitMessage(0);                                     // Correct exit code
            return DefWindowProc(hwnd, uMsg, wParam, lParam);       // Indicate message was handled
        default: return DefWindowProc(hwnd, uMsg, wParam, lParam);  // Pass other messages to default handler
    }
}
}  // namespace JuicyEngine
