#pragma once
#ifdef JE_PLATFORM_WINDOWS
#ifdef JE_BUILD_DLL
#define JE_API __declspec(dllexport)
#else
#define JE_API __declspec(dllimport)
#endif
#else
#error JuicyEngine only supports Windows!
#endif