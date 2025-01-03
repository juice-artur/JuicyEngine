#pragma once

#include <memory>
#include "JuicyEngine/Core/PlatformDetection.h"

#ifdef JE_DEBUG
#if defined(JE_PLATFORM_WINDOWS)
#define JE_DEBUGBREAK() __debugbreak()
#elif defined(JE_PLATFORM_LINUX)
#include <signal.h>
#define JE_DEBUGBREAK() raise(SIGTRAP)
#else
#error "Platform doesn't support debugbreak yet!"
#endif
#define JE_ENABLE_ASSERTS
#else
#define JE_DEBUGBREAK()
#endif

#define JE_EXPAND_MACRO(x) x
#define JE_STRINGIFY_MACRO(x) #x

#define BIT(x) (1 << x)

#define JE_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

namespace JuicyEngine
{
template <typename T>
using Scope = std::unique_ptr<T>;
template <typename T, typename... Args>
constexpr Scope<T> CreateScope(Args&&... args)
{
    return std::make_unique<T>(std::forward<Args>(args)...);
}

template <typename T>
using Ref = std::shared_ptr<T>;
template <typename T, typename... Args>
constexpr Ref<T> CreateRef(Args&&... args)
{
    return std::make_shared<T>(std::forward<Args>(args)...);
}
}  // namespace JuicyEngine

#include "JuicyEngine/Core/Log.h"
#include "JuicyEngine/Core/Assert.h"