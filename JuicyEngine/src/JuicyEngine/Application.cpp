#include "jepch.h"
#include "Application.h"

namespace JuicyEngine
{

Application::Application()
{
    m_Window = Scope<Window>(Window::Create());
}

Application::~Application() {}

void Application::Run()
{
    // TODO: Think about platform agnostic event
    while (m_Running)
    {
        MSG message;
        while (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
        {
            if (message.message == WM_QUIT)
            {
                m_Running = false;
                break;
            }
            TranslateMessage(&message);
            DispatchMessage(&message);
        }
    }
}

}  // namespace JuicyEngine