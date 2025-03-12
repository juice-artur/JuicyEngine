#pragma once


namespace JuicyEngine
{
class Application
{
public:
    Application();
private:
    static Application* AppInstance;
};

Application* CreateApplication();
}