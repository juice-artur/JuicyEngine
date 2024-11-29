#pragma once

#include "JuicyEngine/Core/Core.h"
#include "JuicyEngine/Core/Log.h"
#ifdef JE_ENABLE_ASSERTS
namespace JuicyEngine::Assert
{
// Returns the simple file name rather than full path as suggested by LovelySanta
constexpr const char* CurrentFileName(const char* path)
{
    const char* file = path;
    while (*path)
    {
        if (*path == '/' || *path == '\\')
            file = ++path;
        else
            path++;
    }
    return file;
}
}  // namespace JuicyEngine::Assert
// Alteratively we could use the same "default" message for both "WITH_MSG" and "NO_MSG" and
// provide support for custom formatting by concatenating the formatting string instead of having the format inside the default message
#define JE_INTERNAL_ASSERT_IMPL(type, check, msg, ...)                                                                                     \
    {                                                                                                                                      \
        if (!(check))                                                                                                                      \
        {                                                                                                                                  \
            JE##type##ERROR(msg, __VA_ARGS__);                                                                                             \
            JE_DEBUGBREAK();                                                                                                               \
        }                                                                                                                                  \
    }
#define JE_INTERNAL_ASSERT_WITH_MSG(type, check, ...) JE_INTERNAL_ASSERT_IMPL(type, check, "Assertion failed: {0}", __VA_ARGS__)
#define JE_INTERNAL_ASSERT_NO_MSG(type, check)                                                                                             \
    JE_INTERNAL_ASSERT_IMPL(type, check, "Assertion '{0}' failed at {1}:{2}", JE_STRINGIFY_MACRO(check),                                   \
        ::JuicyEngine::Assert::CurrentFileName(__FILE__), __LINE__)
#define JE_INTERNAL_ASSERT_GET_MACRO_NAME(arg1, arg2, macro, ...) macro
#define JE_INTERNAL_ASSERT_GET_MACRO(...)                                                                                                  \
    JE_EXPAND_MACRO(JE_INTERNAL_ASSERT_GET_MACRO_NAME(__VA_ARGS__, JE_INTERNAL_ASSERT_WITH_MSG, JE_INTERNAL_ASSERT_NO_MSG))
// Currently accepts at least the condition and one additional parameter (the message) being optional
#define JE_ASSERT(...) JE_EXPAND_MACRO(JE_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_, __VA_ARGS__))
#define JE_CORE_ASSERT(...) JE_EXPAND_MACRO(JE_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_CORE_, __VA_ARGS__))
#else
#define JE_ASSERT(...)
#define JE_CORE_ASSERT(...)
#endif