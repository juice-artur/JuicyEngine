#pragma once

#include "Core/Layer.h"

namespace JuicyEngine
{
class Window;
class VulkanContext;

class EditorLayer : public Layer
{
public:
    EditorLayer(Window& Window, VulkanContext& Context);
    ~EditorLayer() override;

    void OnAttach() override;
    void OnDetach() override;
    void OnUpdate() override;
    void OnEvent(Event& event) override;

private:
    Window& m_Window;
    VulkanContext& m_Context;

    float m_FrameTime = 0.0f;
    bool first_frame = true;
    bool m_ViewportHovered = false;
};
} // namespace JuicyEngine
