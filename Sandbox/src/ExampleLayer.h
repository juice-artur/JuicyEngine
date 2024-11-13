#pragma once
#include "JuicyEngine.h"
class ExampleLayer : public JuicyEngine::Layer
{
public:
    ExampleLayer();
    virtual ~ExampleLayer() = default;
    virtual void OnAttach() override;
    virtual void OnDetach() override;
    void OnUpdate(JuicyEngine::Timestep ts) override;
    virtual void OnImGuiRender() override;
    void OnEvent(JuicyEngine::Event& e) override;

private:
    JuicyEngine::ShaderLibrary m_ShaderLibrary;
    JuicyEngine::Ref<JuicyEngine::Shader> m_Shader;
    JuicyEngine::Ref<JuicyEngine::VertexArray> m_VertexArray;
    JuicyEngine::Ref<JuicyEngine::Shader> m_FlatColorShader;
    JuicyEngine::Ref<JuicyEngine::VertexArray> m_SquareVA;
    JuicyEngine::Ref<JuicyEngine::Texture2D> m_Texture, m_ChernoLogoTexture;
    JuicyEngine::OrthographicCameraController m_CameraController;
    glm::vec3 m_SquareColor = {0.2f, 0.3f, 0.8f};
};