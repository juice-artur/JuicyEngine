#pragma once
#ifdef JE_PLATFORM_WINDOWS
extern JuicyEngine::Application* ::JuicyEngine::CreateApplication();
int main(int argc, char** argv)
{
    JuicyEngine::Log::Init();
    JE_CORE_WARN("Initialized Log!");
    int a = 5;
    JE_INFO("Hello! Var={0}", a);
    auto app = JuicyEngine::CreateApplication();
    app->Run();
    delete app;
}
#endif