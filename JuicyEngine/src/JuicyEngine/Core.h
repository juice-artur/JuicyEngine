#pragma once

#include <memory>
// TODO: https://github.com/gabime/spdlog/issues/3251
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
