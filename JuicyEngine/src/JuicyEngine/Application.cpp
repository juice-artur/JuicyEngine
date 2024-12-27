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

void Application::OnEvent(Event& e)
{
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<WindowCloseEvent>(JE_BIND_EVENT_FN(OnWindowClose));
    JE_CORE_TRACE("{0}", e);
}

void Application::Run()
{
    while (m_Running)
    {
        m_Window->OnUpdate();
    }
}

bool Application::OnWindowClose(WindowCloseEvent& e)
{
    m_Running = false;
    return true;
}

}  // namespace JuicyEngine