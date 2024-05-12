#pragma once

#include "Platform.h"
#include <windows.h>

class PlatformWindows : public Platform
{
public:
	JE_API bool Startup(const char* applicationName, int x, int y, int width, int height) override;
	JE_API void Shutdown() override;
	JE_API bool PumpMessages() override;
	JE_API double GetAbsoluteTime() override;

private:
	HINSTANCE hInstance;
	HWND hwnd;
	float clockFrequency;
	LARGE_INTEGER startTime;
};
