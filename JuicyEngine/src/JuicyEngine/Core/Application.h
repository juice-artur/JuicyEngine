#pragma once
#include "JuicyEngine/Core/Core.h"
#include "JuicyEngine/Core/Window.h"
#include "JuicyEngine/Core/LayerStack.h"
#include "JuicyEngine/Events/Event.h"
#include "JuicyEngine/Events/ApplicationEvent.h"
#include "JuicyEngine/Core/Timestep.h"
#include "JuicyEngine/ImGui/ImGuiLayer.h"

namespace JuicyEngine
{
class Application
{
public:
    Application(const std::string& name = "Juicy App");
    virtual ~Application();
    void Run();
    void OnEvent(Event& e);
    void PushLayer(Layer* layer);
    void PushOverlay(Layer* layer);
    inline Window& GetWindow() { return *m_Window; }
    void Close();
    ImGuiLayer* GetImGuiLayer() { return m_ImGuiLayer; }
    inline static Application& Get() { return *s_Instance; }

private:
    bool OnWindowClose(WindowCloseEvent& e);
    bool OnWindowResize(WindowResizeEvent& e);

private:
    Scope<Window> m_Window;
    ImGuiLayer* m_ImGuiLayer;
    bool m_Running = true;
    LayerStack m_LayerStack;
    float m_LastFrameTime = 0.0f;
    bool m_Minimized = false;

private:
    static Application* s_Instance;
};
// To be defined in CLIENT
Application* CreateApplication();
}  // namespace JuicyEngine