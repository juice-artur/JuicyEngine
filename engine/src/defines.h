#pragma once
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#define JE_PLATFORM_WINDOWS 1
#ifndef _WIN64
#error "64-bit is required on Windows!"
#endif
#elif defined(__linux__) || defined(__gnu_linux__)
#define JE_PLATFORM_LINUX 1
#if defined(__ANDROID__)
#define JE_PLATFORM_ANDROID 1
#endif
#else
#error "Unknown platform!"
#endif

#ifdef JE_EXPORT
#ifdef _MSC_VER
#define JE_API __declspec(dllexport)
#else
#define JE_API __attribute__((visibility("default")))
#endif
#else
// Imports
#ifdef _MSC_VER
#define JE_API __declspec(dllimport)
#else
#define JE_API
#endif
#endif