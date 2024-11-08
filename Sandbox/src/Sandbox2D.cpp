
#include "Sandbox2D.h"
#include "imgui/imgui.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Platform/OpenGL/OpenGLShader.h"
Sandbox2D::Sandbox2D() : Layer("Sandbox2D"), m_CameraController(1280.0f / 720.0f) {}
void Sandbox2D::OnAttach() {}
void Sandbox2D::OnDetach()
{
}
void Sandbox2D::OnUpdate(JuicyEngine::Timestep ts)
{
    // Update
    m_CameraController.OnUpdate(ts);
    // Render
    JuicyEngine::RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1});
    JuicyEngine::RenderCommand::Clear();
    JuicyEngine::Renderer2D::BeginScene(m_CameraController.GetCamera());
    JuicyEngine::Renderer2D::DrawQuad({0.0f, 0.0f}, {1.0f, 1.0f}, {0.8f, 0.2f, 0.3f, 1.0f});
    JuicyEngine::Renderer2D::EndScene();
}
void Sandbox2D::OnImGuiRender()
{
    ImGui::Begin("Settings");
    ImGui::ColorEdit4("Square Color", glm::value_ptr(m_SquareColor));
    ImGui::End();
}
void Sandbox2D::OnEvent(JuicyEngine::Event& e)
{
    m_CameraController.OnEvent(e);
}