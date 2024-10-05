#include "jepch.h"
#include "Application.h"
#include "JuicyEngine/Events/ApplicationEvent.h"
#include "JuicyEngine/Log.h"
namespace JuicyEngine {
	Application::Application()
	{
	}
	Application::~Application()
	{
	}
	void Application::Run()
	{
		WindowResizeEvent e(1280, 720);
		if (e.IsInCategory(EventCategoryApplication))
		{
			JE_TRACE(e.ToString());
		}
		if (e.IsInCategory(EventCategoryInput))
		{
			JE_TRACE(e.ToString());
		}
		while (true);
	}
}
