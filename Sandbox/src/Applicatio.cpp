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
		if (JuicyEngine::Input::IsKeyPressed(JE_KEY_TAB))
			JE_TRACE("Tab key is pressed (poll)!");
	}
	void OnEvent(JuicyEngine::Event& event) override
	{
		if (event.GetEventType() == JuicyEngine::EventType::KeyPressed)
		{
			JuicyEngine::KeyPressedEvent& e = (JuicyEngine::KeyPressedEvent&)event;
			if (e.GetKeyCode() == JE_KEY_TAB)
				JE_TRACE("Tab key is pressed (event)!");
			JE_TRACE("{0}", (char)e.GetKeyCode());
		}
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