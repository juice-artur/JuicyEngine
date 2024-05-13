#pragma once

#include "Platform.h"
#include "WindowsWindow.h"

class PlatformWindows : public Platform
{
public:
	JE_API bool Startup(const char* applicationName, int width, int height) override;
	JE_API void Shutdown() override;
	JE_API bool PumpMessages() override;
	JE_API double GetAbsoluteTime() override;

private:
	float clockFrequency;
	LARGE_INTEGER startTime;

	WindowsWindow window;
};
