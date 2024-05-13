#include "PlatformWindows.h"
#include <Log.h>

JE_API bool PlatformWindows::Startup(const char* applicationName, int width, int height)
{
	return window.Create(applicationName, width, height);
}

JE_API void PlatformWindows::Shutdown()
{
	window.Close();
}

JE_API bool PlatformWindows::PumpMessages()
{
	WindowEvent windowEvent;
	while (window.PollEvent(windowEvent))
	{
		switch (windowEvent.type)
		{
			case WET_WindowClose: break;
		}
	}
	return true;
}

JE_API double PlatformWindows::GetAbsoluteTime()
{
	LARGE_INTEGER nowTime;
	QueryPerformanceCounter(&nowTime);
	return (float)nowTime.QuadPart * clockFrequency;
}
