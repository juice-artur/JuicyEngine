#include <JuicyEngine.h>
#include <iostream>


class Sandbox : public JuicyEngine::Application {
 public:
  Sandbox() { std::cout << "Hello, Application!" << std::endl;
  }
  ~Sandbox() {}
};

JuicyEngine::Application* JuicyEngine::CreateApplication() {
  return new Sandbox();
}