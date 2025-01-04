#pragma once

namespace JuicyEngine {
class Application {
 public:
  Application();
  virtual ~Application();
  void Run();
};
// To be defined in CLIENT
Application* CreateApplication();
}  // namespace JuicyEngine