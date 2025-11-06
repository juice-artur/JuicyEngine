#include "JuicyEngine.h"
#include "Core/Input.h"

class ExampleLayer : public JuicyEngine::Layer
{
public:
	ExampleLayer() : Layer("Example") {}

	void OnUpdate() override
	{
		if (JuicyEngine::Input::IsKeyPressed(43))
		{
			JE_TRACE("Tab key is pressed (poll)!");
		}
	}

	void OnEvent(JuicyEngine::Event& event) override
	{
		//   JE_TRACE("{0}", event.Handled);
	}
};

class Sandbox : public JuicyEngine::Application
{
public:
	Sandbox()
	{
		PushLayer(new ExampleLayer());
	}

	~Sandbox() {}
};

JuicyEngine::Application* JuicyEngine::CreateApplication()
{
	return new Sandbox();
}