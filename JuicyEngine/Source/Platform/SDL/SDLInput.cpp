#include "jepch.h"
#include "SDLInput.h"

#include "Core/Application.h"
#include "SDL3/SDL_keyboard.h"
#include "SDL3/SDL_mouse.h"
#include "SDL3/SDL_touch.h"

namespace JuicyEngine
{

Input* Input::s_Instance = new SDLInput();

bool SDLInput::IsKeyPressedImpl(int keycode)
{
    const bool* state = SDL_GetKeyboardState(NULL);
    return state[keycode] != 0;
}

bool SDLInput::IsMouseButtonPressedImpl(int button)
{
    Uint32 buttons = SDL_GetMouseState(nullptr, nullptr);
    return (buttons & SDL_BUTTON_MASK(button)) != 0;
}

std::pair<float, float> SDLInput::GetMousePositionImpl()
{
    float x, y;
    SDL_GetMouseState(&x, &y);
    return {x, y};
}

float SDLInput::GetMouseXImpl()
{
    auto [x, y] = GetMousePositionImpl();
    return x;
}

float SDLInput::GetMouseYImpl()
{
    auto [x, y] = GetMousePositionImpl();
    return y;
}

} // namespace JuicyEngine