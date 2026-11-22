#pragma once

#include <memory>

#include "LayerStack.h"
#include "Events/Event.h"
#include "Window.h"
#include "Events/ApplicationEvent.h"

namespace JuicyEngine
{
class GraphicsContext;
class ImGuiLayer;
class VulkanContext;

class Application
{
public:
    Application();
    virtual ~Application();

    void Run();

    void OnEvent(Event& e);

    void PushLayer(Layer* layer);
    void PushOverlay(Layer* layer);

    Window& GetWindow();
    VulkanContext& GetVulkanContext();

private:
    bool OnWindowClose(WindowCloseEvent& e);

    std::unique_ptr<Window> m_Window;
    std::unique_ptr<GraphicsContext> m_Context;
    bool m_Running = true;
    LayerStack m_LayerStack;
    ImGuiLayer* m_ImGuiLayer = nullptr;
};

// To be defined in CLIENT
Application* CreateApplication();

} // namespace JuicyEngine
