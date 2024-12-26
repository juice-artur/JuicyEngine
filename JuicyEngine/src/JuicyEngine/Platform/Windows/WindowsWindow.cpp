#include "jepch.h"
#include "WindowsWindow.h"

namespace JuicyEngine
{
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
    m_instance = GetModuleHandle(NULL);

    JE_CORE_INFO("Creating window {0} ({1}, {2})", props.Title, props.Width, props.Height);

    // Setup and register window class
    const TCHAR* windowClassName = TEXT("SampleWindowClass");
    const TCHAR* windowName = TEXT("Engine");
    WNDCLASSEX wndClassDescr{};
    wndClassDescr.cbSize = sizeof(WNDCLASSEX);
    wndClassDescr.style = CS_HREDRAW | CS_VREDRAW;
    wndClassDescr.cbClsExtra = 0;
    wndClassDescr.cbWndExtra = 0;
    wndClassDescr.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);

    wndClassDescr.hCursor = LoadCursor(m_instance, IDC_ARROW);
    wndClassDescr.hIcon = LoadIcon(GetModuleHandle(NULL), IDI_APPLICATION);
    wndClassDescr.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

    wndClassDescr.hInstance = m_instance;
    wndClassDescr.lpfnWndProc = WindowProc;
    wndClassDescr.lpszClassName = windowClassName;
    wndClassDescr.lpszMenuName = NULL;

    if (!RegisterClassEx(&wndClassDescr))
    {
        JE_CORE_CRITICAL("Failed to register class");
    }

    HWND hWnd = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, windowClassName, windowName, WS_OVERLAPPEDWINDOW, 320, 180, 1280, 720, NULL, NULL, m_instance, NULL);
    if (hWnd == NULL)
    {
        JE_CORE_CRITICAL("Failed to create window");
    }

    ShowWindow(hWnd, 1);

    m_Window = hWnd;
}

void WindowsWindow::Shutdown() {}

LRESULT WindowsWindow::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_CLOSE:
            DestroyWindow(hWnd);
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
}  // namespace JuicyEngine