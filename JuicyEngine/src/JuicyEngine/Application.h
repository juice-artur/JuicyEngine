#pragma once

#include "Core.h"
#include "Core/Window.h"
#include "Events/ApplicationEvent.h"

namespace JuicyEngine
{

class JE_API Application
{
public:
    Application();
    virtual ~Application();
    void Run();
    void OnEvent(Event& e);

private:
    bool OnWindowClose(WindowCloseEvent& e);
    Scope<Window> m_Window;
    bool m_Running = true;

};

// To be defined in CLIENT
Application* CreateApplication();

}  // namespace JuicyEngine