#include "jepch.h"
#include "OrthographicCameraController.h"
#include "JuicyEngine/Core/Input.h"
#include "JuicyEngine/Core/KeyCodes.h"
namespace JuicyEngine
{
OrthographicCameraController::OrthographicCameraController(float aspectRatio, bool rotation)
    : m_AspectRatio(aspectRatio), m_Camera(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel),
      m_Rotation(rotation)
{
}
void OrthographicCameraController::OnUpdate(Timestep ts)
{
    if (Input::IsKeyPressed(JE_KEY_A))
    {
        m_CameraPosition.x -= cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
        m_CameraPosition.y -= sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
    }
    else if (Input::IsKeyPressed(JE_KEY_D))
    {
        m_CameraPosition.x += cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
        m_CameraPosition.y += sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
    }

    if (Input::IsKeyPressed(JE_KEY_W))
    {
        m_CameraPosition.x += -sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
        m_CameraPosition.y += cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
    }
    else if (Input::IsKeyPressed(JE_KEY_S))
    {
        m_CameraPosition.x -= -sin(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
        m_CameraPosition.y -= cos(glm::radians(m_CameraRotation)) * m_CameraTranslationSpeed * ts;
    }

    if (m_Rotation)
    {
        if (Input::IsKeyPressed(JE_KEY_Q)) m_CameraRotation += m_CameraRotationSpeed * ts;
        if (Input::IsKeyPressed(JE_KEY_E)) m_CameraRotation -= m_CameraRotationSpeed * ts;

        if (m_CameraRotation > 180.0f)
            m_CameraRotation -= 360.0f;
        else if (m_CameraRotation <= -180.0f)
            m_CameraRotation += 360.0f;

        m_Camera.SetRotation(m_CameraRotation);
    }
}
void OrthographicCameraController::OnEvent(Event& e)
{
    EventDispatcher dispatcher(e);
    dispatcher.Dispatch<MouseScrolledEvent>(JE_BIND_EVENT_FN(OrthographicCameraController::OnMouseScrolled));
    dispatcher.Dispatch<WindowResizeEvent>(JE_BIND_EVENT_FN(OrthographicCameraController::OnWindowResized));
}
bool OrthographicCameraController::OnMouseScrolled(MouseScrolledEvent& e)
{
    m_ZoomLevel -= e.GetYOffset() * 0.25f;
    m_ZoomLevel = std::max(m_ZoomLevel, 0.25f);
    m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
    return false;
}
bool OrthographicCameraController::OnWindowResized(WindowResizeEvent& e)
{
    m_AspectRatio = (float)e.GetWidth() / (float)e.GetHeight();
    m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
    return false;
}
}  // namespace JuicyEngine