#include "Application.h"
#include "jepch.h"

#include "ImGui/ImGuiLayer.h"
#include "Editor/EditorLayer.h"
#include "Platform/Vulkan/VulkanContext.h"

namespace JuicyEngine
{

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

	Application* s_Instance = nullptr;

	Application::Application()
	{
		JE_CORE_ASSERT(!s_Instance, "Application already exists!")
		s_Instance = this;

		m_Window = std::unique_ptr<Window>(Window::Create());
		m_Window->SetEventCallback(BIND_EVENT_FN(OnEvent));

		auto* Context = dynamic_cast<VulkanContext*>(VulkanContext::Get());
		Context->Init(m_Window->GetNativeWindow());

		ImGuiLayer* ImGui = new ImGuiLayer();
		PushOverlay(ImGui);
		PushLayer(new EditorLayer());
	}

	Application::~Application()
	{
		auto* Context = dynamic_cast<VulkanContext*>(VulkanContext::Get());
		vkDeviceWaitIdle(Context->GetDevice()->GetLogicalDevice());

		m_LayerStack.Cleanup();

		Context->Shutdown();
	}

	Application& Application::Get()
	{
		JE_CORE_ASSERT(s_Instance, "Application not created!")
		return *s_Instance;
	}

	Window& Application::GetWindow()
	{
		return *m_Window;
	}

	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
	}

	void Application::PushOverlay(Layer* layer)
	{
		m_LayerStack.PushOverlay(layer);
		if (auto* ImGui = dynamic_cast<ImGuiLayer*>(layer))
			m_ImGuiLayer = ImGui;
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
		auto* Context = dynamic_cast<VulkanContext*>(VulkanContext::Get());

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

			Context->Draw();
			Context->SwapBuffers();
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}

} // namespace JuicyEngine