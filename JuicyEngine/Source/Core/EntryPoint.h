#pragma once

#if defined(__APPLE__)
#include <SDL3/SDL_main.h>
#endif

extern JuicyEngine::Application* JuicyEngine::CreateApplication();

int main(int argc, char** argv)
{
    JuicyEngine::Log::Init();

    auto app = JuicyEngine::CreateApplication();
    app->Run();
    delete app;
}