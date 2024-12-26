#pragma once

#include "Core.h"
#include "Core/Window.h"

namespace JuicyEngine
{

class JE_API Application
{
public:
    Application();
    virtual ~Application();

    void Run();

private:
    Scope<Window> m_Window;
    bool m_Running = true;

};

// To be defined in CLIENT
Application* CreateApplication();

}  // namespace JuicyEngine