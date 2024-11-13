#include "jepch.h"
#include "JuicyEngine/Core/Input.h"
#ifdef JE_PLATFORM_WINDOWS
#include "Platform/Windows/WindowsInput.h"
#endif
namespace JuicyEngine
{
Scope<Input> Input::s_Instance = Input::Create();
Scope<Input> Input::Create()
{
#ifdef JE_PLATFORM_WINDOWS
    return CreateScope<WindowsInput>();
#else
    JE_CORE_ASSERT(false, "Unknown platform!");
    return nullptr;
#endif
}
}  // namespace JuicyEngine