#pragma once
#include "Core.h"
#include "Window.h"
#include "JuicyEngine/LayerStack.h"
#include "JuicyEngine/Events/Event.h"
#include "JuicyEngine/Events/ApplicationEvent.h"
#include "JuicyEngine/ImGui/ImGuiLayer.h"

#include "JuicyEngine/Renderer/Shader.h"
#include "JuicyEngine/Renderer/Buffer.h"
#include "JuicyEngine/Renderer/VertexArray.h"

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
    inline Window& GetWindow() { return *m_Window; }
    inline static Application& Get() { return *s_Instance; }

private:
    bool OnWindowClose(WindowCloseEvent& e);
    std::unique_ptr<Window> m_Window;
    ImGuiLayer* m_ImGuiLayer;
    bool m_Running = true;
    LayerStack m_LayerStack;

    std::shared_ptr<Shader> m_Shader;
    std::shared_ptr<VertexArray> m_VertexArray;
    std::shared_ptr<Shader> m_BlueShader;
    std::shared_ptr<VertexArray> m_SquareVA;

private:
    static Application* s_Instance;
};
// To be defined in CLIENT
Application* CreateApplication();
}  // namespace JuicyEngine