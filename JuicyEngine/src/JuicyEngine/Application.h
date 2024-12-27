#pragma once

#include "Core.h"
#include "Core/LayerStack.h"
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
    void PushLayer(Layer* layer);
    void PushOverlay(Layer* layer);

private:
    bool OnWindowClose(WindowCloseEvent& e);
    Scope<Window> m_Window;
    bool m_Running = true;
    LayerStack m_LayerStack;

};

// To be defined in CLIENT
Application* CreateApplication();

}  // namespace JuicyEngine