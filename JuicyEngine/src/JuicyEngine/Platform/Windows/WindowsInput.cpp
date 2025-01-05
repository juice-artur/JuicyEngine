#include "jepch.h"
#include "JuicyEngine/Core/Input.h"
#include "JuicyEngine/Core/Application.h"
#include <Windows.h>

namespace JuicyEngine
{
bool Input::IsKeyPressed(const KeyCode key)
{
    // Check if the key is pressed using GetAsyncKeyState
    return (GetAsyncKeyState(static_cast<int>(key)) & 0x8000) != 0;
}

bool Input::IsMouseButtonPressed(const MouseCode button)
{
    // Check if the mouse button is pressed using GetAsyncKeyState
    int buttonCode = 0;
    switch (button)
    {
        case Mouse::ButtonLeft: buttonCode = VK_LBUTTON; break;
        case Mouse::ButtonRight: buttonCode = VK_RBUTTON; break;
        case Mouse::ButtonMiddle: buttonCode = VK_MBUTTON; break;
        default: return false;
    }

    return (GetAsyncKeyState(buttonCode) & 0x8000) != 0;
}

 glm::vec2 Input::GetMousePosition()
{
     // Get the current cursor position using GetCursorPos
     POINT point;
     if (GetCursorPos(&point))
     {
         return {(float)point.x, (float)point.y};
     }

     return {0.0f, 0.0f};
 }

float Input::GetMouseX()
{
     return GetMousePosition().x;
}

float Input::GetMouseY()
{
     return GetMousePosition().y;
}
}  // namespace JuicyEngine
