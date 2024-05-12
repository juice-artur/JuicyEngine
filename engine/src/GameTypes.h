#pragma once

#include "core/application.h"

/**
 * Represents the basic game state in a game.
 * Called for creation by the application.
 */
class Game
{
public:
	ApplicationConfig app_config;
	bool (*initialize)();

	bool (*update)(float delta_time);

	bool (*render)(float delta_time);

	void (*on_resize)(unsigned int width, unsigned int height);

	void* state;
};