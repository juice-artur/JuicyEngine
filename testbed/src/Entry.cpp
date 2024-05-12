#include "game.h"

#include <Entry.h>

#include <platform/platform.h>

// Define the function to create a game
bool CreateGame(Game* out_game)
{
	// Application configuration.
	out_game->app_config.startPosX = 100;
	out_game->app_config.startPosY = 100;
	out_game->app_config.startWidth = 1280;
	out_game->app_config.startHeight = 720;
	out_game->app_config.name = "Test Engine Testbed";
	out_game->update = game_update;
	out_game->render = game_render;
	out_game->initialize = game_initialize;
	out_game->on_resize = game_on_resize;


	return true;
}