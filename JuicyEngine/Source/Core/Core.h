#pragma once

#include <cstdlib>

#define JE_ENABLE_ASSERTS

#ifdef JE_ENABLE_ASSERTS
namespace JuicyEngine
{
inline void JEDebugBreak()
{
#if defined(_MSC_VER)
    __debugbreak();
#elif defined(__clang__) || defined(__GNUC__)
    __builtin_trap();
#else
    std::abort();
#endif
}
} // namespace JuicyEngine

#define JE_ASSERT(x, ...)                                                                                              \
    {                                                                                                                  \
        if (!(x))                                                                                                      \
        {                                                                                                              \
            JE_ERROR("Assertion Failed: {0}", __VA_ARGS__);                                                            \
            ::JuicyEngine::JEDebugBreak();                                                                             \
        }                                                                                                              \
    }
#define JE_CORE_ASSERT(x, ...)                                                                                         \
    {                                                                                                                  \
        if (!(x))                                                                                                      \
        {                                                                                                              \
            JE_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__);                                                       \
            ::JuicyEngine::JEDebugBreak();                                                                             \
        }                                                                                                              \
    }
#else
#define JE_ASSERT(x, ...)
#define JE_CORE_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)