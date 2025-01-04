#include <JuicyEngine.h>
#include <iostream>


class Sandbox : public JuicyEngine::Application {
 public:
  Sandbox() { JE_CORE_INFO("Hello, JuicyEngine!");
  }
  ~Sandbox() {}
};

JuicyEngine::Application* JuicyEngine::CreateApplication() {
  return new Sandbox();
}