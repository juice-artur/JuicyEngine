#include <EntryPoint.h>
#include <iostream>


class Sandbox : public JuicyEngine::Application
{
public:
    Sandbox()
    {
        std::cout << "test";
    }
};

JuicyEngine::Application* JuicyEngine::CreateApplication()
{
    return new Sandbox();
}