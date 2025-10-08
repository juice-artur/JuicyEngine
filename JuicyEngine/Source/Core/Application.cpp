#include <print>
#include "SDL3/SDL.h"

void SayHello()
{
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        std::print("Failed init SDL");
    }

    SDL_Window* Window = SDL_CreateWindow("JuicyEngine",
        320, 240, SDL_WINDOW_RESIZABLE | SDL_WINDOW_BORDERLESS);
    
    SDL_Delay(5000);
    SDL_Quit();
    std::print("{2} {1}{0}!\n", 23, "C++", "Hello");
}