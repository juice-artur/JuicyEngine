#pragma once

#include "Core/Layer.h"

namespace JuicyEngine
{
class EditorLayer : public Layer
{
public:
    EditorLayer();
    ~EditorLayer() override;

    void OnAttach() override;
    void OnDetach() override;
    void OnUpdate() override;
    void OnEvent(Event& event) override;

private:
    float m_FrameTime = 0.0f;
    bool first_frame = true;
};
}
