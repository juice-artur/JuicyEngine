#pragma once

#include <memory>

#ifdef JE_PLATFORM_WINDOWS
#if JE_DYNAMIC_LINK
#ifdef JE_BUILD_DLL
#define JE_API __declspec(dllexport)
#else
#define JE_API __declspec(dllimport)
#endif
#else
#define JE_API
#endif
#else
#error JuicyEngine only supports Windows!
#endif

#ifdef JE_DEBUG
#define JE_ENABLE_ASSERTS
#endif

#ifdef JE_ENABLE_ASSERTS
#define JE_ASSERT(x, ...)                                                                                                                  \
    {                                                                                                                                      \
        if (!(x))                                                                                                                          \
        {                                                                                                                                  \
            JE_ERROR("Assertion Failed: {0}", __VA_ARGS__);                                                                                \
            __debugbreak();                                                                                                                \
        }                                                                                                                                  \
    }
#define JE_CORE_ASSERT(x, ...)                                                                                                             \
    {                                                                                                                                      \
        if (!(x))                                                                                                                          \
        {                                                                                                                                  \
            JE_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__);                                                                           \
            __debugbreak();                                                                                                                \
        }                                                                                                                                  \
    }
#else
#define JE_ASSERT(x, ...)
#define JE_CORE_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)

#define JE_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)

namespace JuicyEngine
{
template <typename T>
using Scope = std::unique_ptr<T>;
template <typename T>
using Ref = std::shared_ptr<T>;
}  // namespace JuicyEngine