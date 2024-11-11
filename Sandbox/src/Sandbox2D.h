#pragma once
#include "JuicyEngine.h"
class Sandbox2D : public JuicyEngine::Layer
{
public:
    Sandbox2D();
    virtual ~Sandbox2D() = default;
    virtual void OnAttach() override;
    virtual void OnDetach() override;
    void OnUpdate(JuicyEngine::Timestep ts) override;
    virtual void OnImGuiRender() override;
    void OnEvent(JuicyEngine::Event& e) override;

private:
    JuicyEngine::OrthographicCameraController m_CameraController;

    // Temp
    JuicyEngine::Ref<JuicyEngine::VertexArray> m_SquareVA;
    JuicyEngine::Ref<JuicyEngine::Shader> m_FlatColorShader;
    JuicyEngine::Ref<JuicyEngine::Texture2D> m_CheckerboardTexture;
    glm::vec4 m_SquareColor = {0.2f, 0.3f, 0.8f, 1.0f};
};