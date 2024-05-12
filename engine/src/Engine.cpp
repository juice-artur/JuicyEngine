#include <stdio.h>
#include "Core/Log.h"
#include "Engine.h"
#include "Platform.h"

#if JE_PLATFORM_WINDOWS
#include "Platform/Windows/PlatformWindows.h"
#endif

JE_API bool Engine::Init()
{
	Log::Init();
	LOG_TRACE("Engine::Init()");
#if JE_PLATFORM_WINDOWS
	platform = new PlatformWindows();
#endif

	if (!platform->Startup("JE Engine", 100, 100, 1280, 720))
	{
		return false;
	}
	return true;
}

JE_API void Engine::Tick()
{
	while (true)
	{
		platform->PumpMessages();
	}
	platform->Shutdown();
}
