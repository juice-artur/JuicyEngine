#pragma once

extern JuicyEngine::Application* ::JuicyEngine::CreateApplication();

int main(int argc, char** argv) {

  auto app = JuicyEngine::CreateApplication();
  app->Run();
  delete app;
}