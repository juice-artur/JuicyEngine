#pragma once

namespace JuicyEngine
{
using MouseCode = int;

namespace Mouse
{
enum : MouseCode
{
    // Windows-specific mouse button codes
    ButtonLeft = VK_LBUTTON,    // Left Mouse Button
    ButtonRight = VK_RBUTTON,   // Right Mouse Button
    ButtonMiddle = VK_MBUTTON,  // Middle Mouse Button
};
}  // namespace Mouse
}  // namespace JuicyEngine
