#pragma once

namespace JuicyEngine
{
typedef enum class MouseCode : uint16_t
{
    // From glfw3.h
    Button0 = 0,
    Button1 = 1,
    Button2 = 2,
    Button3 = 3,
    Button4 = 4,
    Button5 = 5,
    Button6 = 6,
    Button7 = 7,

    ButtonLast = Button7,
    ButtonLeft = Button0,
    ButtonRight = Button1,
    ButtonMiddle = Button2
} Mouse;

inline std::ostream& operator<<(std::ostream& os, MouseCode mouseCode)
{
    os << static_cast<int32_t>(mouseCode);
    return os;
}
}  // namespace JuicyEngine

#define HZ_MOUSE_BUTTON_0 ::JuicyEngine::Mouse::Button0
#define HZ_MOUSE_BUTTON_1 ::JuicyEngine::Mouse::Button1
#define HZ_MOUSE_BUTTON_2 ::JuicyEngine::Mouse::Button2
#define HZ_MOUSE_BUTTON_3 ::JuicyEngine::Mouse::Button3
#define HZ_MOUSE_BUTTON_4 ::JuicyEngine::Mouse::Button4
#define HZ_MOUSE_BUTTON_5 ::JuicyEngine::Mouse::Button5
#define HZ_MOUSE_BUTTON_6 ::JuicyEngine::Mouse::Button6
#define HZ_MOUSE_BUTTON_7 ::JuicyEngine::Mouse::Button7
#define HZ_MOUSE_BUTTON_LAST ::JuicyEngine::Mouse::ButtonLast
#define HZ_MOUSE_BUTTON_LEFT ::JuicyEngine::Mouse::ButtonLeft
#define HZ_MOUSE_BUTTON_RIGHT ::JuicyEngine::Mouse::ButtonRight
#define HZ_MOUSE_BUTTON_MIDDLE ::JuicyEngine::Mouse::ButtonMiddle