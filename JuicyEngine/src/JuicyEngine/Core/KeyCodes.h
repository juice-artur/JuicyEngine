#pragma once

namespace JuicyEngine
{
using KeyCode = int;

namespace Key
{
enum : KeyCode
{
    // Mapping from GLFW to Windows Virtual Key codes
    Space = VK_SPACE,
    Apostrophe = 0xDE,       // '
    Comma = VK_OEM_COMMA,    // ,
    Minus = VK_OEM_MINUS,    // -
    Period = VK_OEM_PERIOD,  // .
    Slash = VK_OEM_2,        // /

    D0 = '0',  // 0
    D1 = '1',  // 1
    D2 = '2',  // 2
    D3 = '3',  // 3
    D4 = '4',  // 4
    D5 = '5',  // 5
    D6 = '6',  // 6
    D7 = '7',  // 7
    D8 = '8',  // 8
    D9 = '9',  // 9

    A = 'A',
    B = 'B',
    C = 'C',
    D = 'D',
    E = 'E',
    F = 'F',
    G = 'G',
    H = 'H',
    I = 'I',
    J = 'J',
    K = 'K',
    L = 'L',
    M = 'M',
    N = 'N',
    O = 'O',
    P = 'P',
    Q = 'Q',
    R = 'R',
    S = 'S',
    T = 'T',
    U = 'U',
    V = 'V',
    W = 'W',
    X = 'X',
    Y = 'Y',
    Z = 'Z',

    LeftBracket = VK_OEM_4,  // [
    Backslash = VK_OEM_5,    // \
    RightBracket = VK_OEM_6, // ]
    GraveAccent = VK_OEM_3,  // `

    World1 = 0xA1,  // non-US #1
    World2 = 0xA2,  // non-US #2

    // Function keys
    Escape = VK_ESCAPE,
    Enter = VK_RETURN,
    Tab = VK_TAB,
    Backspace = VK_BACK,
    Insert = VK_INSERT,
    Delete = VK_DELETE,
    Right = VK_RIGHT,
    Left = VK_LEFT,
    Down = VK_DOWN,
    Up = VK_UP,
    PageUp = VK_PRIOR,
    PageDown = VK_NEXT,
    Home = VK_HOME,
    End = VK_END,
    CapsLock = VK_CAPITAL,
    ScrollLock = VK_SCROLL,
    NumLock = VK_NUMLOCK,
    PrintScreen = VK_SNAPSHOT,
    Pause = VK_PAUSE,
    F1 = VK_F1,
    F2 = VK_F2,
    F3 = VK_F3,
    F4 = VK_F4,
    F5 = VK_F5,
    F6 = VK_F6,
    F7 = VK_F7,
    F8 = VK_F8,
    F9 = VK_F9,
    F10 = VK_F10,
    F11 = VK_F11,
    F12 = VK_F12,
    F13 = VK_F13,
    F14 = VK_F14,
    F15 = VK_F15,
    F16 = VK_F16,
    F17 = VK_F17,
    F18 = VK_F18,
    F19 = VK_F19,
    F20 = VK_F20,
    F21 = VK_F21,
    F22 = VK_F22,
    F23 = VK_F23,
    F24 = VK_F24,

    // Keypad
    KP0 = VK_NUMPAD0,
    KP1 = VK_NUMPAD1,
    KP2 = VK_NUMPAD2,
    KP3 = VK_NUMPAD3,
    KP4 = VK_NUMPAD4,
    KP5 = VK_NUMPAD5,
    KP6 = VK_NUMPAD6,
    KP7 = VK_NUMPAD7,
    KP8 = VK_NUMPAD8,
    KP9 = VK_NUMPAD9,
    KPDecimal = VK_DECIMAL,
    KPDivide = VK_DIVIDE,
    KPMultiply = VK_MULTIPLY,
    KPSubtract = VK_SUBTRACT,
    KPAdd = VK_ADD,
    KPEnter = VK_RETURN,
    KPEqual = VK_OEM_PLUS,

    LeftShift = VK_LSHIFT,
    LeftControl = VK_LCONTROL,
    LeftAlt = VK_LMENU,
    LeftSuper = VK_LWIN,
    RightShift = VK_RSHIFT,
    RightControl = VK_RCONTROL,
    RightAlt = VK_RMENU,
    RightSuper = VK_RWIN,
    Menu = VK_APPS
};
}  // namespace Key
}  // namespace JuicyEngine
