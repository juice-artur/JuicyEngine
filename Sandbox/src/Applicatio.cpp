#include <JuicyEngine.h>

#include "Platform/OpenGL/OpenGLShader.h"
#include "imgui/imgui.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
class ExampleLayer : public JuicyEngine::Layer
{
public:
    ExampleLayer() : Layer("Example"), m_Camera(-1.6f, 1.6f, -0.9f, 0.9f), m_CameraPosition(0.0f)
    {
        m_VertexArray.reset(JuicyEngine::VertexArray::Create());
        float vertices[3 * 7] = {-0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f, 0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f, 0.0f, 0.5f, 0.0f,
            0.8f, 0.8f, 0.2f, 1.0f};
        JuicyEngine::Ref<JuicyEngine::VertexBuffer> vertexBuffer;
        vertexBuffer.reset(JuicyEngine::VertexBuffer::Create(vertices, sizeof(vertices)));
        JuicyEngine::BufferLayout layout = {
            {JuicyEngine::ShaderDataType::Float3, "a_Position"}, {JuicyEngine::ShaderDataType::Float4, "a_Color"}};
        vertexBuffer->SetLayout(layout);
        m_VertexArray->AddVertexBuffer(vertexBuffer);
        uint32_t indices[3] = {0, 1, 2};
        JuicyEngine::Ref<JuicyEngine::IndexBuffer> indexBuffer;
        indexBuffer.reset(JuicyEngine::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
        m_VertexArray->SetIndexBuffer(indexBuffer);
        m_SquareVA.reset(JuicyEngine::VertexArray::Create());
        float squareVertices[5 * 4] = {
            -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.5f, 0.5f, 0.0f, 1.0f, 1.0f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f};
        JuicyEngine::Ref<JuicyEngine::VertexBuffer> squareVB;
        squareVB.reset(JuicyEngine::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
        squareVB->SetLayout({{JuicyEngine::ShaderDataType::Float3, "a_Position"}, {JuicyEngine::ShaderDataType::Float2, "a_TexCoord"}});
        m_SquareVA->AddVertexBuffer(squareVB);
        uint32_t squareIndices[6] = {0, 1, 2, 2, 3, 0};
        JuicyEngine::Ref<JuicyEngine::IndexBuffer> squareIB;
        squareIB.reset(JuicyEngine::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
        m_SquareVA->SetIndexBuffer(squareIB);
        std::string vertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;
			uniform mat4 u_ViewProjection;
            uniform mat4 u_Transform;
			out vec3 v_Position;
			out vec4 v_Color;
			void main()
			{
				v_Position = a_Position;
				v_Color = a_Color;
				gl_Position = u_ViewProjection * u_Transform  * vec4(a_Position, 1.0);	
			}
		)";
        std::string fragmentSrc = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;
			in vec3 v_Position;
			in vec4 v_Color;
			void main()
			{
				color = vec4(v_Position * 0.5 + 0.5, 1.0);
				color = v_Color;
			}
		)";
        m_Shader.reset(JuicyEngine::Shader::Create(vertexSrc, fragmentSrc));
        std::string flatColorShaderVertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;
			uniform mat4 u_ViewProjection;
            uniform mat4 u_Transform;
			out vec3 v_Position;
			void main()
			{
				v_Position = a_Position;
				gl_Position = u_ViewProjection * u_Transform *  vec4(a_Position, 1.0);	
			}
		)";
        std::string flatColorShaderFragmentSrc = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;
			in vec3 v_Position;
            uniform vec3 u_Color;
			void main()
			{
				color = vec4(u_Color, 1.0);
			}
		)";
        m_FlatColorShader.reset(JuicyEngine::Shader::Create(flatColorShaderVertexSrc, flatColorShaderFragmentSrc));

        m_TextureShader.reset(JuicyEngine::Shader::Create("assets/shaders/Texture.glsl"));
        m_Texture = JuicyEngine::Texture2D::Create("assets/textures/Checkerboard.png");
        m_JuicyLogoTexture = JuicyEngine::Texture2D::Create("assets/textures/JE_Logo.png");
        std::dynamic_pointer_cast<JuicyEngine::OpenGLShader>(m_TextureShader)->Bind();
        std::dynamic_pointer_cast<JuicyEngine::OpenGLShader>(m_TextureShader)->UploadUniformInt("u_Texture", 0);
    }
    void OnUpdate(JuicyEngine::Timestep ts) override
    {
        if (JuicyEngine::Input::IsKeyPressed(JE_KEY_LEFT))
            m_CameraPosition.x -= m_CameraMoveSpeed * ts;
        else if (JuicyEngine::Input::IsKeyPressed(JE_KEY_RIGHT))
            m_CameraPosition.x += m_CameraMoveSpeed * ts;
        if (JuicyEngine::Input::IsKeyPressed(JE_KEY_UP))
            m_CameraPosition.y += m_CameraMoveSpeed * ts;
        else if (JuicyEngine::Input::IsKeyPressed(JE_KEY_DOWN))
            m_CameraPosition.y -= m_CameraMoveSpeed * ts;
        if (JuicyEngine::Input::IsKeyPressed(JE_KEY_A)) m_CameraRotation += m_CameraRotationSpeed * ts;
        if (JuicyEngine::Input::IsKeyPressed(JE_KEY_D)) m_CameraRotation -= m_CameraRotationSpeed * ts;
        JuicyEngine::RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1});
        JuicyEngine::RenderCommand::Clear();
        m_Camera.SetPosition(m_CameraPosition);
        m_Camera.SetRotation(m_CameraRotation);
        JuicyEngine::Renderer::BeginScene(m_Camera);
        glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));
        std::dynamic_pointer_cast<JuicyEngine::OpenGLShader>(m_FlatColorShader)->Bind();
        std::dynamic_pointer_cast<JuicyEngine::OpenGLShader>(m_FlatColorShader)->UploadUniformFloat3("u_Color", m_SquareColor);
        for (int y = 0; y < 20; y++)
        {
            for (int x = 0; x < 20; x++)
            {
                glm::vec3 pos(x * 0.11f, y * 0.11f, 0.0f);
                glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;
                JuicyEngine::Renderer::Submit(m_FlatColorShader, m_SquareVA, transform);
            }
        }
        m_Texture->Bind();
        JuicyEngine::Renderer::Submit(m_TextureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));
        m_JuicyLogoTexture->Bind();
        JuicyEngine::Renderer::Submit(m_TextureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));
        JuicyEngine::Renderer::EndScene();
    }
    void OnEvent(JuicyEngine::Event& event) override {}
    virtual void OnImGuiRender() override
    {
        ImGui::Begin("Settings");
        ImGui::ColorEdit3("Square Color", glm::value_ptr(m_SquareColor));
        ImGui::End();
    }

private:
    JuicyEngine::Ref<JuicyEngine::Shader> m_Shader;
    JuicyEngine::Ref<JuicyEngine::VertexArray> m_VertexArray;
    JuicyEngine::Ref<JuicyEngine::Shader> m_FlatColorShader, m_TextureShader;
    JuicyEngine::Ref<JuicyEngine::VertexArray> m_SquareVA;
    JuicyEngine::Ref<JuicyEngine::Texture2D> m_Texture, m_JuicyLogoTexture;
    JuicyEngine::OrthographicCamera m_Camera;
    glm::vec3 m_CameraPosition;
    float m_CameraMoveSpeed = 5.0f;

    float m_CameraRotation = 0.0f;
    float m_CameraRotationSpeed = 180.0f;
    glm::vec3 m_SquareColor = {0.2f, 0.3f, 0.8f};
};

class Sandbox : public JuicyEngine::Application
{
public:
    Sandbox() { PushLayer(new ExampleLayer()); }
    ~Sandbox() {}
};

JuicyEngine::Application* JuicyEngine::CreateApplication()
{
    return new Sandbox();
}