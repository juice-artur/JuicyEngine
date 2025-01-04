#include "Application.h"

#include <JuicyEngine/Events/ApplicationEvent.h>

#include "Log.h"

namespace JuicyEngine
{
Application::Application()
{
    m_Window = std::unique_ptr<Window>(Window::Create());
}
Application::~Application() {}
void Application::Run()
{
    while (m_Running)
    {
        m_Window->OnUpdate();
    }
}
}  // namespace JuicyEngine