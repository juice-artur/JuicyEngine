#pragma once

#include "JuicyEngine/Core/Core.h"
#include "Window.h"
#include "JuicyEngine/Core/LayerStack.h"
#include "JuicyEngine/Events/Event.h"
#include "JuicyEngine/Events/ApplicationEvent.h"
#include <JuicyEngine/Renderer/GraphicsContext.h>

namespace JuicyEngine
{
class Application
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
    bool OnWindowResize(WindowResizeEvent& e);
    std::unique_ptr<Window> m_Window;
    std::unique_ptr<GraphicsContext> m_Context;
    bool m_Running = true;
    LayerStack m_LayerStack;
};
// To be defined in CLIENT
Application* CreateApplication();
}  // namespace JuicyEngine