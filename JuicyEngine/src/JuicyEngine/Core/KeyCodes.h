#pragma once

namespace JuicyEngine
{
typedef enum class KeyCode : uint16_t
{
    // From glfw3.h
    Space = 32,
    Apostrophe = 39, /* ' */
    Comma = 44,      /* , */
    Minus = 45,      /* - */
    Period = 46,     /* . */
    Slash = 47,      /* / */

    D0 = 48, /* 0 */
    D1 = 49, /* 1 */
    D2 = 50, /* 2 */
    D3 = 51, /* 3 */
    D4 = 52, /* 4 */
    D5 = 53, /* 5 */
    D6 = 54, /* 6 */
    D7 = 55, /* 7 */
    D8 = 56, /* 8 */
    D9 = 57, /* 9 */

    Semicolon = 59, /* ; */
    Equal = 61,     /* = */

    A = 65,
    B = 66,
    C = 67,
    D = 68,
    E = 69,
    F = 70,
    G = 71,
    H = 72,
    I = 73,
    J = 74,
    K = 75,
    L = 76,
    M = 77,
    N = 78,
    O = 79,
    P = 80,
    Q = 81,
    R = 82,
    S = 83,
    T = 84,
    U = 85,
    V = 86,
    W = 87,
    X = 88,
    Y = 89,
    Z = 90,

    LeftBracket = 91,  /* [ */
    Backslash = 92,    /* \ */
    RightBracket = 93, /* ] */
    GraveAccent = 96,  /* ` */

    World1 = 161, /* non-US #1 */
    World2 = 162, /* non-US #2 */

    /* Function keys */
    Escape = 256,
    Enter = 257,
    Tab = 258,
    Backspace = 259,
    Insert = 260,
    Delete = 261,
    Right = 262,
    Left = 263,
    Down = 264,
    Up = 265,
    PageUp = 266,
    PageDown = 267,
    Home = 268,
    End = 269,
    CapsLock = 280,
    ScrollLock = 281,
    NumLock = 282,
    PrintScreen = 283,
    Pause = 284,
    F1 = 290,
    F2 = 291,
    F3 = 292,
    F4 = 293,
    F5 = 294,
    F6 = 295,
    F7 = 296,
    F8 = 297,
    F9 = 298,
    F10 = 299,
    F11 = 300,
    F12 = 301,
    F13 = 302,
    F14 = 303,
    F15 = 304,
    F16 = 305,
    F17 = 306,
    F18 = 307,
    F19 = 308,
    F20 = 309,
    F21 = 310,
    F22 = 311,
    F23 = 312,
    F24 = 313,
    F25 = 314,

    /* Keypad */
    KP0 = 320,
    KP1 = 321,
    KP2 = 322,
    KP3 = 323,
    KP4 = 324,
    KP5 = 325,
    KP6 = 326,
    KP7 = 327,
    KP8 = 328,
    KP9 = 329,
    KPDecimal = 330,
    KPDivide = 331,
    KPMultiply = 332,
    KPSubtract = 333,
    KPAdd = 334,
    KPEnter = 335,
    KPEqual = 336,

    LeftShift = 340,
    LeftControl = 341,
    LeftAlt = 342,
    LeftSuper = 343,
    RightShift = 344,
    RightControl = 345,
    RightAlt = 346,
    RightSuper = 347,
    Menu = 348
} Key;

inline std::ostream& operator<<(std::ostream& os, KeyCode keyCode)
{
    os << static_cast<int32_t>(keyCode);
    return os;
}
}  // namespace JuicyEngine

// From glfw3.h
#define JE_KEY_SPACE ::JuicyEngine::Key::Space
#define JE_KEY_APOSTROPHE ::JuicyEngine::Key::Apostrophe /* ' */
#define JE_KEY_COMMA ::JuicyEngine::Key::Comma           /* , */
#define JE_KEY_MINUS ::JuicyEngine::Key::Minus           /* - */
#define JE_KEY_PERIOD ::JuicyEngine::Key::Period         /* . */
#define JE_KEY_SLASH ::JuicyEngine::Key::Slash           /* / */
#define JE_KEY_0 ::JuicyEngine::Key::D0
#define JE_KEY_1 ::JuicyEngine::Key::D1
#define JE_KEY_2 ::JuicyEngine::Key::D2
#define JE_KEY_3 ::JuicyEngine::Key::D3
#define JE_KEY_4 ::JuicyEngine::Key::D4
#define JE_KEY_5 ::JuicyEngine::Key::D5
#define JE_KEY_6 ::JuicyEngine::Key::D6
#define JE_KEY_7 ::JuicyEngine::Key::D7
#define JE_KEY_8 ::JuicyEngine::Key::D8
#define JE_KEY_9 ::JuicyEngine::Key::D9
#define JE_KEY_SEMICOLON ::JuicyEngine::Key::Semicolon /* ; */
#define JE_KEY_EQUAL ::JuicyEngine::Key::Equal         /* = */
#define JE_KEY_A ::JuicyEngine::Key::A
#define JE_KEY_B ::JuicyEngine::Key::B
#define JE_KEY_C ::JuicyEngine::Key::C
#define JE_KEY_D ::JuicyEngine::Key::D
#define JE_KEY_E ::JuicyEngine::Key::E
#define JE_KEY_F ::JuicyEngine::Key::F
#define JE_KEY_G ::JuicyEngine::Key::G
#define JE_KEY_H ::JuicyEngine::Key::H
#define JE_KEY_I ::JuicyEngine::Key::I
#define JE_KEY_J ::JuicyEngine::Key::J
#define JE_KEY_K ::JuicyEngine::Key::K
#define JE_KEY_L ::JuicyEngine::Key::L
#define JE_KEY_M ::JuicyEngine::Key::M
#define JE_KEY_N ::JuicyEngine::Key::N
#define JE_KEY_O ::JuicyEngine::Key::O
#define JE_KEY_P ::JuicyEngine::Key::P
#define JE_KEY_Q ::JuicyEngine::Key::Q
#define JE_KEY_R ::JuicyEngine::Key::R
#define JE_KEY_S ::JuicyEngine::Key::S
#define JE_KEY_T ::JuicyEngine::Key::T
#define JE_KEY_U ::JuicyEngine::Key::U
#define JE_KEY_V ::JuicyEngine::Key::V
#define JE_KEY_W ::JuicyEngine::Key::W
#define JE_KEY_X ::JuicyEngine::Key::X
#define JE_KEY_Y ::JuicyEngine::Key::Y
#define JE_KEY_Z ::JuicyEngine::Key::Z
#define JE_KEY_LEFT_BRACKET ::JuicyEngine::Key::LeftBracket   /* [ */
#define JE_KEY_BACKSLASH ::JuicyEngine::Key::Backslash        /* \ */
#define JE_KEY_RIGHT_BRACKET ::JuicyEngine::Key::RightBracket /* ] */
#define JE_KEY_GRAVE_ACCENT ::JuicyEngine::Key::GraveAccent   /* ` */
#define JE_KEY_WORLD_1 ::JuicyEngine::Key::World1             /* non-US #1 */
#define JE_KEY_WORLD_2 ::JuicyEngine::Key::World2             /* non-US #2 */

/* Function keys */
#define JE_KEY_ESCAPE ::JuicyEngine::Key::Escape
#define JE_KEY_ENTER ::JuicyEngine::Key::Enter
#define JE_KEY_TAB ::JuicyEngine::Key::Tab
#define JE_KEY_BACKSPACE ::JuicyEngine::Key::Backspace
#define JE_KEY_INSERT ::JuicyEngine::Key::Insert
#define JE_KEY_DELETE ::JuicyEngine::Key::Delete
#define JE_KEY_RIGHT ::JuicyEngine::Key::Right
#define JE_KEY_LEFT ::JuicyEngine::Key::Left
#define JE_KEY_DOWN ::JuicyEngine::Key::Down
#define JE_KEY_UP ::JuicyEngine::Key::Up
#define JE_KEY_PAGE_UP ::JuicyEngine::Key::PageUp
#define JE_KEY_PAGE_DOWN ::JuicyEngine::Key::PageDown
#define JE_KEY_HOME ::JuicyEngine::Key::Home
#define JE_KEY_END ::JuicyEngine::Key::End
#define JE_KEY_CAPS_LOCK ::JuicyEngine::Key::CapsLock
#define JE_KEY_SCROLL_LOCK ::JuicyEngine::Key::ScrollLock
#define JE_KEY_NUM_LOCK ::JuicyEngine::Key::NumLock
#define JE_KEY_PRINT_SCREEN ::JuicyEngine::Key::PrintScreen
#define JE_KEY_PAUSE ::JuicyEngine::Key::Pause
#define JE_KEY_F1 ::JuicyEngine::Key::F1
#define JE_KEY_F2 ::JuicyEngine::Key::F2
#define JE_KEY_F3 ::JuicyEngine::Key::F3
#define JE_KEY_F4 ::JuicyEngine::Key::F4
#define JE_KEY_F5 ::JuicyEngine::Key::F5
#define JE_KEY_F6 ::JuicyEngine::Key::F6
#define JE_KEY_F7 ::JuicyEngine::Key::F7
#define JE_KEY_F8 ::JuicyEngine::Key::F8
#define JE_KEY_F9 ::JuicyEngine::Key::F9
#define JE_KEY_F10 ::JuicyEngine::Key::F10
#define JE_KEY_F11 ::JuicyEngine::Key::F11
#define JE_KEY_F12 ::JuicyEngine::Key::F12
#define JE_KEY_F13 ::JuicyEngine::Key::F13
#define JE_KEY_F14 ::JuicyEngine::Key::F14
#define JE_KEY_F15 ::JuicyEngine::Key::F15
#define JE_KEY_F16 ::JuicyEngine::Key::F16
#define JE_KEY_F17 ::JuicyEngine::Key::F17
#define JE_KEY_F18 ::JuicyEngine::Key::F18
#define JE_KEY_F19 ::JuicyEngine::Key::F19
#define JE_KEY_F20 ::JuicyEngine::Key::F20
#define JE_KEY_F21 ::JuicyEngine::Key::F21
#define JE_KEY_F22 ::JuicyEngine::Key::F22
#define JE_KEY_F23 ::JuicyEngine::Key::F23
#define JE_KEY_F24 ::JuicyEngine::Key::F24
#define JE_KEY_F25 ::JuicyEngine::Key::F25

/* Keypad */
#define JE_KEY_KP_0 ::JuicyEngine::Key::KP0
#define JE_KEY_KP_1 ::JuicyEngine::Key::KP1
#define JE_KEY_KP_2 ::JuicyEngine::Key::KP2
#define JE_KEY_KP_3 ::JuicyEngine::Key::KP3
#define JE_KEY_KP_4 ::JuicyEngine::Key::KP4
#define JE_KEY_KP_5 ::JuicyEngine::Key::KP5
#define JE_KEY_KP_6 ::JuicyEngine::Key::KP6
#define JE_KEY_KP_7 ::JuicyEngine::Key::KP7
#define JE_KEY_KP_8 ::JuicyEngine::Key::KP8
#define JE_KEY_KP_9 ::JuicyEngine::Key::KP9
#define JE_KEY_KP_DECIMAL ::JuicyEngine::Key::KPDecimal
#define JE_KEY_KP_DIVIDE ::JuicyEngine::Key::KPDivide
#define JE_KEY_KP_MULTIPLY ::JuicyEngine::Key::KPMultiply
#define JE_KEY_KP_SUBTRACT ::JuicyEngine::Key::KPSubtract
#define JE_KEY_KP_ADD ::JuicyEngine::Key::KPAdd
#define JE_KEY_KP_ENTER ::JuicyEngine::Key::KPEnter
#define JE_KEY_KP_EQUAL ::JuicyEngine::Key::KPEqual

#define JE_KEY_LEFT_SHIFT ::JuicyEngine::Key::LeftShift
#define JE_KEY_LEFT_CONTROL ::JuicyEngine::Key::LeftControl
#define JE_KEY_LEFT_ALT ::JuicyEngine::Key::LeftAlt
#define JE_KEY_LEFT_SUPER ::JuicyEngine::Key::LeftSuper
#define JE_KEY_RIGHT_SHIFT ::JuicyEngine::Key::RightShift
#define JE_KEY_RIGHT_CONTROL ::JuicyEngine::Key::RightControl
#define JE_KEY_RIGHT_ALT ::JuicyEngine::Key::RightAlt
#define JE_KEY_RIGHT_SUPER ::JuicyEngine::Key::RightSuper
#define JE_KEY_MENU ::JuicyEngine::Key::Menu