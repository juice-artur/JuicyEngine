#pragma once

#include <functional>
#include <string>
#include "spdlog/fmt/bundled/format.h"
#include <format>
#include <ostream>

#include "Core/Core.h"

namespace JuicyEngine
{
enum class EventType
{
    None = 0,
    WindowClose,
    WindowResize,
    WindowFocus,
    WindowLostFocus,
    WindowMoved,
    AppTick,
    AppUpdate,
    AppRender,
    KeyPressed,
    KeyReleased,
    MouseButtonPressed,
    MouseButtonReleased,
    MouseMoved,
    MouseScrolled
};

enum EventCategory
{
    None = 0,
    EventCategoryApplication = BIT(0),
    EventCategoryInput =  BIT(1),
    EventCategoryKeyboard =  BIT(2),
    EventCategoryMouse =  BIT(3),
    EventCategoryMouseButton =  BIT(4)
};

#define EVENT_CLASS_TYPE(type) static EventType GetStaticType() { return EventType::##type; }\
virtual EventType GetEventType() const override { return GetStaticType(); }\
virtual const char* GetName() const override { return #type; }

#define EVENT_CLASS_CATEGORY(category) virtual int GetCategoryFlags() const override { return category; }

class Event
{
public:
    bool Handled = false;

public:
    virtual EventType GetEventType() const = 0;
    virtual const char* GetName() const = 0;
    virtual int GetCategoryFlags() const = 0;
    virtual std::string ToString() const { return GetName(); }

    inline bool IsInCategory(EventCategory category)
    {
        return GetCategoryFlags() & category;
    }
};

class EventDispatcher
{
    template <typename T>
    using EventFn = std::function<bool(T&)>;

public:
    EventDispatcher(Event& event)
        : m_Event(event)
    {
    }

    template <typename T>
    bool Dispatch(EventFn<T> func)
    {
        if (m_Event.GetEventType() == T::GetStaticType())
        {
            m_Event.Handled = func(*(T*)&m_Event);
            return true;
        }
        return false;
    }

private:
    Event& m_Event;
};

inline std::ostream& operator<<(std::ostream& os, const Event& e)
{
    return os << e.ToString();
}
}

template <typename T>
struct fmt::formatter<T, std::enable_if_t<std::is_base_of_v<JuicyEngine::Event, T>, char>> : fmt::formatter<std::string>
{
    template <typename FormatContext>
    auto format(const T& event, FormatContext& ctx) const -> decltype(ctx.out())
    {
        return fmt::formatter<std::string>::format(event.ToString(), ctx);
    }
};