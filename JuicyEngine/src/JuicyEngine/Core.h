#pragma once


//TODO: https://github.com/gabime/spdlog/issues/3251
#define FMT_UNICODE 0

#ifdef JE_PLATFORM_WINDOWS
#ifdef JE_BUILD_DLL
#define JE_API __declspec(dllexport)
#else
#define JE_API __declspec(dllimport)
#endif
#else
#error JuicyEngine only supports Windows!
#endif