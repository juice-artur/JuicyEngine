#include "test.h"
#include <stdio.h>
#include "core/log.h"
#include "SDL3/SDL.h"
#include <SDL_init.h>
void print_int(int i)
{
	Log::Init();
	LOG_TRACE("The number is: {0}", i);

	 SDL_Window* window;	 // Declare a pointer

	SDL_Init(SDL_INIT_VIDEO);  // Initialize SDL2

	// Create an application window with the following settings:
	window = SDL_CreateWindow("An SDL3 window",	 // window title
		640,									 // width, in pixels
		480,									 // height, in pixels
		SDL_WINDOW_OPENGL						 // flags - see below
	);

	// Check that the window was successfully created
	if (window == NULL)
	{
		// In the case that the window could not be made...
		SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Could not create window: %s\n", SDL_GetError());
	}

	// The window is open: could enter program loop here (see SDL_PollEvent())

	SDL_Delay(3000);  // Pause execution for 3000 milliseconds, for example

	// Close and destroy the window
	SDL_DestroyWindow(window);

	// Clean up
	SDL_Quit();
}