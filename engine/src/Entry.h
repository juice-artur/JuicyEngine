#pragma once

#include "Core/Application.h"
// #include "Log.h"

extern bool CreateGame(Game* outGame);

/**
 * The main entry point of the application.
 */
int main(void)
{
	Game game_inst;
	if (!CreateGame(&game_inst))
	{
		// LOG_CRITICAL("Could not create game!");
		return -1;
	}

	// Ensure the function pointers exist.
	if (!game_inst.render || !game_inst.update || !game_inst.initialize || !game_inst.on_resize)
	{
		// LOG_CRITICAL("The game's function pointers must be assigned!");
		return -2;
	}

	Application application;
	if (!application.Init(&game_inst))
	{
		// LOG_INFO("Application failed to create!.");
		return 1;
	}

	application.Tick();

	return 0;
}