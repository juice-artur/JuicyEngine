#pragma once
#include "JuicyEngine/Core/Layer.h"
#include "JuicyEngine/Events/ApplicationEvent.h"
#include <JuicyEngine/Events/KeyEvent.h>
#include <JuicyEngine/Events/MouseEvent.h>
namespace JuicyEngine
{
class ImGuiLayer : public Layer
{
public:
    ImGuiLayer();
    ~ImGuiLayer() = default;
    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void OnEvent(Event& e) override;
    void Begin();
    void End();
    void BlockEvents(bool block) { m_BlockEvents = block; }

private:
    bool m_BlockEvents = true;
    float m_Time = 0.0f;
};
}  // namespace JuicyEngine