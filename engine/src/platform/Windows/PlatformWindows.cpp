#include "PlatformWindows.h"
#include <Log.h>

LRESULT CALLBACK win32_process_message(HWND hwnd, unsigned int msg, WPARAM w_param, LPARAM l_param)
{
	switch (msg)
	{
		case WM_ERASEBKGND: return 1;
		case WM_CLOSE: return 0;
		case WM_DESTROY: PostQuitMessage(0); return 0;
		case WM_SIZE:
		{
		}
		break;
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:
		case WM_KEYUP:
		case WM_SYSKEYUP:
		{
		}
		break;
		case WM_MOUSEMOVE:
		{
		}
		break;
		case WM_MOUSEWHEEL:
		{
		}
		break;
		case WM_LBUTTONDOWN:
		case WM_MBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_LBUTTONUP:
		case WM_MBUTTONUP:
		case WM_RBUTTONUP:
		{
		}
		break;
	}

	return DefWindowProcA(hwnd, msg, w_param, l_param);
}

JE_API bool PlatformWindows::Startup(const char* applicationName, int x, int y, int width, int height)
{
	hInstance = GetModuleHandleA(0);

	HICON icon = LoadIcon(hInstance, IDI_APPLICATION);
	WNDCLASSA wc;
	memset(&wc, 0, sizeof(wc));
	wc.style = CS_DBLCLKS;
	wc.lpfnWndProc = win32_process_message;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = icon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = NULL;
	wc.lpszClassName = "je_window_class";

	if (!RegisterClassA(&wc))
	{
		MessageBoxA(0, "Window registration failed", "Error", MB_ICONEXCLAMATION | MB_OK);
		return FALSE;
	}

	// Create window
	unsigned int clientX = x;
	unsigned int clientY = y;
	unsigned int clientWidth = width;
	unsigned int clientHeight = height;

	unsigned int windowX = clientX;
	unsigned int windowY = clientY;
	unsigned int windowWidth = clientWidth;
	unsigned int windowHeight = clientHeight;

	unsigned int windowStyle = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION;
	unsigned int windowExStyle = WS_EX_APPWINDOW;

	windowStyle |= WS_MAXIMIZEBOX;
	windowStyle |= WS_MINIMIZEBOX;
	windowStyle |= WS_THICKFRAME;

	RECT border_rect = {0, 0, 0, 0};
	AdjustWindowRectEx(&border_rect, windowStyle, 0, windowExStyle);

	windowX += border_rect.left;
	windowY += border_rect.top;

	windowWidth += border_rect.right - border_rect.left;
	windowHeight += border_rect.bottom - border_rect.top;

	HWND handle = CreateWindowExA(
		windowExStyle, "je_window_class", applicationName, windowStyle, windowX, windowY, windowWidth, windowHeight, 0, 0, hInstance, 0);

	if (handle == 0)
	{
		MessageBoxA(NULL, "Window creation failed!", "Error!", MB_ICONEXCLAMATION | MB_OK);

		LOG_CRITICAL("Window creation failed!");
		return FALSE;
	}
	else
	{
		hwnd = handle;
	}

	bool should_activate = 1;
	int show_window_command_flags = should_activate ? SW_SHOW : SW_SHOWNOACTIVATE;
	ShowWindow(hwnd, show_window_command_flags);

	LARGE_INTEGER frequency;
	QueryPerformanceFrequency(&frequency);
	clockFrequency = 1.0 / (float)frequency.QuadPart;
	QueryPerformanceCounter(&startTime);

	return TRUE;
}

JE_API void PlatformWindows::Shutdown()
{
	if (hwnd)
	{
		DestroyWindow(hwnd);
		hwnd = nullptr;
	}
}

JE_API bool PlatformWindows::PumpMessages()
{
	MSG message;
	while (PeekMessageA(&message, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&message);
		DispatchMessageA(&message);
	}

	return true;
}

JE_API double PlatformWindows::GetAbsoluteTime()
{
	LARGE_INTEGER nowTime;
	QueryPerformanceCounter(&nowTime);
	return (float)nowTime.QuadPart * clockFrequency;
}
