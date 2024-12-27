#include "jepch.h"
#include "Application.h"

namespace JuicyEngine
{

Application::Application()
{
    m_Window = Scope<Window>(Window::Create());
    m_Window->SetEventCallback(JE_BIND_EVENT_FN(OnEvent));
}

Application::~Application() {}

void Application::PushLayer(Layer* layer)
{
    m_LayerStack.PushLayer(layer);
}
void Application::PushOverlay(Layer* layer)
{
    m_LayerStack.PushOverlay(layer);
}

void Application::OnEvent(Event& e)
{
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<WindowCloseEvent>(JE_BIND_EVENT_FN(OnWindowClose));
    for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();)
    {
        (*--it)->OnEvent(e);
        if (e.Handled)
            break;
    }
}

void Application::Run()
{
    while (m_Running)
    {
        m_Window->OnUpdate();
        for (Layer* layer : m_LayerStack)
        {
            layer->OnUpdate();
        }
    }
}

bool Application::OnWindowClose(WindowCloseEvent& e)
{
    m_Running = false;
    return true;
}

}  // namespace JuicyEngine