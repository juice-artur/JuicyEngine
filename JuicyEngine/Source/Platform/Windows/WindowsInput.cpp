#include "jepch.h"
#include "WindowsInput.h"

#include "Core/Application.h"
#include "SDL3/SDL_keyboard.h"
#include "SDL3/SDL_mouse.h"


namespace JuicyEngine
{

Input* Input::s_Instance = new WindowsInput();

bool WindowsInput::IsKeyPressedImpl(int keycode)
{
    const bool* state = SDL_GetKeyboardState(NULL);
    return state[keycode] != 0;
}

bool WindowsInput::IsMouseButtonPressedImpl(int button)
{
    Uint32 buttons = SDL_GetMouseState(nullptr, nullptr);
    return (buttons & SDL_BUTTON_MASK(button)) != 0;
}

std::pair<float, float> WindowsInput::GetMousePositionImpl()
{
    float x, y;
    SDL_GetMouseState(&x, &y);
    return {x, y};
}

float WindowsInput::GetMouseXImpl()
{
    auto [x, y] = GetMousePositionImpl();
    return x;
}

float WindowsInput::GetMouseYImpl()
{
    auto [x, y] = GetMousePositionImpl();
    return y;
}

}