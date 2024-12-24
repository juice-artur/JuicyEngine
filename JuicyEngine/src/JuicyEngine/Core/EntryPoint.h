#pragma once
#include "JuicyEngine/Core/Core.h"

#ifdef JE_PLATFORM_WINDOWS
extern JuicyEngine::Application* ::JuicyEngine::CreateApplication(ApplicationCommandLineArgs args);
int main(int argc, char** argv)
{
    JuicyEngine::Log::Init();
    JE_CORE_WARN("Initialized Log!");
    int a = 5;
    JE_INFO("Hello! Var={0}", a);
    auto app = JuicyEngine::CreateApplication({argc, argv});
    app->Run();
    delete app;
}
#endif