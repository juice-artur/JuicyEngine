#pragma once

#include "JuicyEngine/Core/KeyCodes.h"
#include "JuicyEngine/Core/MouseCodes.h"

namespace JuicyEngine
{
class Input
{
public:
public:
    static bool IsKeyPressed(KeyCode key);

    static bool IsMouseButtonPressed(MouseCode button);
    // static glm::vec2 GetMousePosition();
    static float GetMouseX();
    static float GetMouseY();
};
}  // namespace JuicyEngine