#pragma once

#include <CoreDefines.h>
#include <GameTypes.h>

typedef struct game_state
{
	float delta_time;
} game_state;

bool game_initialize();

bool game_update(float delta_time);

bool game_render(float delta_time);

void game_on_resize(unsigned int width, unsigned int height);