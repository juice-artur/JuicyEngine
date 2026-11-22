#include "Application.h"
#include "jepch.h"

#include "ImGui/ImGuiLayer.h"
#include "Renderer/GraphicsContext.h"
#include "Platform/Vulkan/VulkanContext.h"

namespace JuicyEngine
{

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

Application::Application()
{
    m_Window = std::unique_ptr<Window>(Window::Create());
    m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));

    m_Context = GraphicsContext::Create();

    auto* Vulkan = static_cast<VulkanContext*>(m_Context.get());
    Vulkan->Init(m_Window->GetNativeWindow());

    m_ImGuiLayer = new ImGuiLayer(*m_Window, *Vulkan);
    PushOverlay(m_ImGuiLayer);
}

Application::~Application()
{
    auto* Vulkan = static_cast<VulkanContext*>(m_Context.get());

    vkDeviceWaitIdle(Vulkan->GetDevice()->GetLogicalDevice());
    Vulkan->DestroyViewportFramebuffer();

    m_LayerStack.Cleanup();

    Vulkan->Shutdown();
}

Window& Application::GetWindow()
{
    return *m_Window;
}

VulkanContext& Application::GetVulkanContext()
{
    return *static_cast<VulkanContext*>(m_Context.get());
}

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
        m_Window->OnUpdate();

        if (m_ImGuiLayer)
            m_ImGuiLayer->Begin();

        for (Layer* layer : m_LayerStack)
        {
            layer->OnUpdate();
        }

        if (m_ImGuiLayer)
            m_ImGuiLayer->Render();

        m_Context->Draw();
        m_Context->SwapBuffers();
    }
}

bool Application::OnWindowClose(WindowCloseEvent&)
{
    m_Running = false;
    return true;
}

} // namespace JuicyEngine
