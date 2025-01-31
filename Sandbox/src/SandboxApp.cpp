#include <JuicyEngine.h>


class ExampleLayer : public JuicyEngine::Layer
{
public:
    ExampleLayer() : Layer("Example") {}
    void OnUpdate() override
    {
        if (JuicyEngine::Input::IsKeyPressed(JuicyEngine::Key::A))
        {
            JE_TRACE("A is pressed!");
        }
    }
    void OnEvent(JuicyEngine::Event& event) override {}
};

class Sandbox : public JuicyEngine::Application
{
public:
    Sandbox() { PushLayer(new ExampleLayer()); }
    ~Sandbox() {}
};

JuicyEngine::Application* JuicyEngine::CreateApplication()
{
    return new Sandbox();
}