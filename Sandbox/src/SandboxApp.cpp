#include <JuicyEngine.h>
#include <JuicyEngine/Core/EntryPoint.h>
#include "imgui/imgui.h"
#include "ExampleLayer.h"
#include "Sandbox2D.h"

class Sandbox : public JuicyEngine::Application
{
public:
    Sandbox(JuicyEngine::ApplicationCommandLineArgs args)
    {  // PushLayer(new ExampleLayer());
        PushLayer(new Sandbox2D());
    }
    ~Sandbox() {}
};

JuicyEngine::Application* JuicyEngine::CreateApplication(JuicyEngine::ApplicationCommandLineArgs args)
{
    return new Sandbox(args);
}