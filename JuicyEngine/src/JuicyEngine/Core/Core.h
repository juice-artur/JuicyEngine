#pragma once

#include <memory>

#ifdef _WIN32
#ifdef _WIN64
#define JE_PLATFORM_WINDOWS
#else
#error "x86 Builds are not supported!"
#endif
#elif defined(__APPLE__) || defined(__MACH__)
#include <TargetConditionals.h>
/* TARGET_OS_MAC exists on all the platforms
 * so we must check all of them (in this order)
 * to ensure that we're running on MAC
 * and not some other Apple platform */
#if TARGET_IPHONE_SIMULATOR == 1
#error "IOS simulator is not supported!"
#elif TARGET_OS_IPHONE == 1
#define JE_PLATFORM_IOS
#error "IOS is not supported!"
#elif TARGET_OS_MAC == 1
#define JE_PLATFORM_MACOS
#error "MacOS is not supported!"
#else
#error "Unknown Apple platform!"
#endif
/* We also have to check __ANDROID__ before __linux__
 * since android is based on the linux kernel
 * it has __linux__ defined */
#elif defined(__ANDROID__)
#define JE_PLATFORM_ANDROID
#error "Android is not supported!"
#elif defined(__linux__)
#define JE_PLATFORM_LINUX
#error "Linux is not supported!"
#else
/* Unknown compiler/platform */
#error "Unknown platform!"
#endif  // End of platform detection

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

#ifdef JE_ENABLE_ASSERTS
#define JE_ASSERT(x, ...)                                                                                                                  \
    {                                                                                                                                      \
        if (!(x))                                                                                                                          \
        {                                                                                                                                  \
            JE_ERROR("Assertion Failed: {0}", __VA_ARGS__);                                                                                \
            JE_DEBUGBREAK();                                                                                                               \
        }                                                                                                                                  \
    }
#define JE_CORE_ASSERT(x, ...)                                                                                                             \
    {                                                                                                                                      \
        if (!(x))                                                                                                                          \
        {                                                                                                                                  \
            JE_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__);                                                                           \
            JE_DEBUGBREAK();                                                                                                               \
        }                                                                                                                                  \
    }
#else
#define JE_ASSERT(x, ...)
#define JE_CORE_ASSERT(x, ...)
#endif

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