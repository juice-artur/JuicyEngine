#pragma once
#ifdef JE_PLATFORM_WINDOWS
extern JuicyEngine::Application* ::JuicyEngine::CreateApplication();
int main(int argc, char** argv)
{
	auto app = JuicyEngine::CreateApplication();
	app->Run();
	delete app;
}
#endif