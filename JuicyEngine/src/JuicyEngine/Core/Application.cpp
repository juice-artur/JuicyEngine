#include "jepch.h"

#include "Application.h"

#include "Log.h"
#include "JuicyEngine/RHI/VulkanRHI/VulkanContext.h"

namespace JuicyEngine
{
#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

Application::Application()
{
    m_Window = std::unique_ptr<Window>(Window::Create());
    m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));

    m_Context = std::unique_ptr<GraphicsContext>(new VulkanContext(m_Window.get()));
    m_Context->Init();
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
    dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));
    dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(Application::OnWindowResize));
    for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();)
    {
        (*--it)->OnEvent(e);
        if (e.Handled)
        {
            break;
        }
    }
}
void Application::Run()
{
    while (m_Running)
    {
        for (Layer* layer : m_LayerStack)
        {
            layer->OnUpdate();
        }
        m_Context->DrawFrame();
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
    JE_CORE_TRACE("WindowResizeEvent: {0}, {1}", e.GetWidth(), e.GetHeight());
    m_Context->RecreateSwapchain();
    return false;
}

}  // namespace JuicyEngine