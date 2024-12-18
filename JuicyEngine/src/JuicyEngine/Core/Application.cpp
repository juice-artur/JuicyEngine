#include "jepch.h"
#include "JuicyEngine/Core/Application.h"
#include "JuicyEngine/Core/Log.h"

#include "JuicyEngine/Core/Input.h"

#include <GLFW/glfw3.h>

#include "JuicyEngine/Renderer/Renderer.h"

namespace JuicyEngine
{

Application* Application::s_Instance = nullptr;

Application::Application(const std::string& name)
{
    JE_CORE_ASSERT(!s_Instance, "Application already exists!");
    s_Instance = this;
    m_Window = Scope<Window>(Window::Create(WindowProps(name)));
    m_Window->SetEventCallback(JE_BIND_EVENT_FN(Application::OnEvent));
    Renderer::Init();
    m_ImGuiLayer = new ImGuiLayer();
    PushOverlay(m_ImGuiLayer);
}
Application::~Application()
{
    Renderer::Shutdown();
}

void Application::PushLayer(Layer* layer)
{
    m_LayerStack.PushLayer(layer);
    layer->OnAttach();
}
void Application::PushOverlay(Layer* layer)
{
    m_LayerStack.PushOverlay(layer);
    layer->OnAttach();
}

void Application::Close() 
{
    m_Running = false;
}

void Application::OnEvent(Event& e)
{
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<WindowCloseEvent>(JE_BIND_EVENT_FN(Application::OnWindowClose));
    dispatcher.Dispatch<WindowResizeEvent>(JE_BIND_EVENT_FN(Application::OnWindowResize));
    for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it)
    {
        if (e.Handled)
        {
            break;
        }
        (*it)->OnEvent(e);
    }
}

void Application::Run()
{
    while (m_Running)
    {
        float time = (float)glfwGetTime();
        Timestep timestep = time - m_LastFrameTime;
        m_LastFrameTime = time;

        if (!m_Minimized)
        {
            for (Layer* layer : m_LayerStack)
                layer->OnUpdate(timestep);
        }
        m_ImGuiLayer->Begin();
        for (Layer* layer : m_LayerStack)
            layer->OnImGuiRender();
        m_ImGuiLayer->End();
        m_Window->OnUpdate();
    }
}

bool Application::OnWindowClose(WindowCloseEvent& e)
{
    m_Running = false;
    return true;
}
bool Application::OnWindowResize(WindowResizeEvent& e)
{
    if (e.GetWidth() == 0 || e.GetHeight() == 0)
    {
        m_Minimized = true;
        return false;
    }
    m_Minimized = false;
    Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());
    return false;
}
}  // namespace JuicyEngine