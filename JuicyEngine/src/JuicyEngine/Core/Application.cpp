#include "Application.h"
#include <iostream>

namespace JuicyEngine {
Application::Application() {}
Application::~Application() {}
void Application::Run() 
{
  std::cout << "Hello, JuicyEngine!" << std::endl;
}
}  // namespace JuicyEngine