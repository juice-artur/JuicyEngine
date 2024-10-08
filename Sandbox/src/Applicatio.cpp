#include <jepch.h>
#include <JuicyEngine.h>

class ExampleLayer : public JuicyEngine::Layer
{
public:
	ExampleLayer()
		: Layer("Example")
	{
	}
	void OnUpdate() override
	{
		JE_INFO("ExampleLayer::Update");
	}
	void OnEvent(JuicyEngine::Event& event) override
	{
		JE_TRACE("{0}", event);
	}
};

class Sandbox : public JuicyEngine::Application
{
public:
	Sandbox()
	{
		PushLayer(new ExampleLayer());
		PushOverlay(new JuicyEngine::ImGuiLayer());
	}
	~Sandbox()
	{
	}
};

JuicyEngine::Application* JuicyEngine::CreateApplication()
{
	return new Sandbox();
}