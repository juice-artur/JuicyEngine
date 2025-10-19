#pragma once

#define JE_ENABLE_ASSERTS

#ifdef JE_ENABLE_ASSERTS
    #define JE_ASSERT(x, ...) { if(!(x)) { JE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
    #define JE_CORE_ASSERT(x, ...) { if(!(x)) { JE_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
    #define JE_ASSERT(x, ...)
    #define JE_CORE_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)