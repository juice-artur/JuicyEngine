#include <stdio.h>
#include "Core/Log.h"
#include "Application.h"
#include "Platform.h"
#include "GameTypes.h"

#if JE_PLATFORM_WINDOWS
#include "Platform/Windows/PlatformWindows.h"
#endif

JE_API bool Application::Init(Game* gameInst)
{
	Log::Init();
	LOG_TRACE("Engine::Init()");
#if JE_PLATFORM_WINDOWS
	platform = new PlatformWindows();
#endif

	if (initialized)
	{
		LOG_CRITICAL("application_create called more than once.");
		return FALSE;
	}

	this->gameInst = gameInst;

	LOG_CRITICAL("A test message: {0}", 3.14f);
	LOG_ERROR("A test message: {0}", 3.14f);
	LOG_WARN("A test message: {0}", 3.14f);
	LOG_INFO("A test message: {0}", 3.14f);
	LOG_TRACE("A test message: {0}", 3.14f);

	is_running = TRUE;
	is_suspended = FALSE;

	if (!platform->Startup(gameInst->app_config.name, gameInst->app_config.startPosX, gameInst->app_config.startPosY,
			gameInst->app_config.startWidth, gameInst->app_config.startHeight))
	{
		return FALSE;
	}

	if (!this->gameInst)
	{
		LOG_CRITICAL("Game failed to initialize.");
		return FALSE;
	}

	this->gameInst->on_resize(width, height);

	initialized = TRUE;

	return TRUE;
}

JE_API void Application::Tick()
{
	while (true)
	{
		platform->PumpMessages();
	}
	platform->Shutdown();

	while (is_running)
	{
		if (!platform->PumpMessages())
		{
			is_running = FALSE;
		}

		if (!is_suspended)
		{
			if (!gameInst->update(0))
			{
				LOG_CRITICAL("Game update failed, shutting down.");
				is_running = FALSE;
				break;
			}

			// Call the game's render routine.
			if (!gameInst->render(0))
			{
				LOG_CRITICAL("Game render failed, shutting down.");
				is_running = FALSE;
				break;
			}
		}
	}

	is_running = FALSE;

	platform->Shutdown();

	// return TRUE;
}
