#pragma once

#include "JuicyEngine/Core/Core.h"
#include "JuicyEngine/Events/Event.h"
#include "JuicyEngine/Events/ApplicationEvent.h"
#include "Window.h"

namespace JuicyEngine
{
class Application
{
public:
    Application();
    virtual ~Application();
    void Run();
    void OnEvent(Event& e);

private:

    bool OnWindowClose(WindowCloseEvent& e);
    std::unique_ptr<Window> m_Window;
    bool m_Running = true;
};
// To be defined in CLIENT
Application* CreateApplication();
}  // namespace JuicyEngine