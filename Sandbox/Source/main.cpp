#include "JuicyEngine.h"

class Sandbox : public JuicyEngine::Application
{
public:
    Sandbox()
    {

    }

    ~Sandbox()
    {

    }

};

JuicyEngine::Application* JuicyEngine::CreateApplication()
{
    return new Sandbox();
}