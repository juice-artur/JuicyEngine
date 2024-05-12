#include <stdio.h>
#include "Core/log.h"
#include "SDL3/SDL.h"
#include <SDL_init.h>
void print_int(int i)
{
	Log::Init();
	LOG_TRACE("The number is: {0}", i);

	SDL_Window* window;

	SDL_Init(SDL_INIT_VIDEO);

	window = SDL_CreateWindow("An SDL3 window", 640, 480, SDL_WINDOW_OPENGL);

	if (window == NULL)
	{
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not create window: %s\n", SDL_GetError());
	}

	SDL_Delay(3000);

	SDL_DestroyWindow(window);
	SDL_Quit();

	if (platform_startup(&state, "Kohi Engine Testbed", 100, 100, 1280, 720))
	{
		while (TRUE)
		{
			platform_pump_messages(&state);
		}
	}
	platform_shutdown(&state);
}
