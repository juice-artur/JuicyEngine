#pragma once
#include "JuicyEngine/Core/Core.h"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

namespace JuicyEngine
{
class  Log
{
public:
    static void Init();
    inline static Ref<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
    inline static Ref<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }

private:
    static Ref<spdlog::logger> s_CoreLogger;
    static Ref<spdlog::logger> s_ClientLogger;
};
}  // namespace JuicyEngine
// Core log macros
#define JE_CORE_TRACE(...) ::JuicyEngine::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define JE_CORE_INFO(...) ::JuicyEngine::Log::GetCoreLogger()->info(__VA_ARGS__)
#define JE_CORE_WARN(...) ::JuicyEngine::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define JE_CORE_ERROR(...) ::JuicyEngine::Log::GetCoreLogger()->error(__VA_ARGS__)
#define JE_CORE_CRITICAL(...) ::JuicyEngine::Log::GetCoreLogger()->critical(__VA_ARGS__)
// Client log macros
#define JE_TRACE(...) ::JuicyEngine::Log::GetClientLogger()->trace(__VA_ARGS__)
#define JE_INFO(...) ::JuicyEngine::Log::GetClientLogger()->info(__VA_ARGS__)
#define JE_WARN(...) ::JuicyEngine::Log::GetClientLogger()->warn(__VA_ARGS__)
#define JE_ERROR(...) ::JuicyEngine::Log::GetClientLogger()->error(__VA_ARGS__)
#define JE_FATAL(...) ::JuicyEngine::Log::GetClientLogger()->fatal(__VA_ARGS__)