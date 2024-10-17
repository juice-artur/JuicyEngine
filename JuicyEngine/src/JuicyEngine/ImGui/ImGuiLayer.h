#pragma once
#include "JuicyEngine/Layer.h"
#include "JuicyEngine/Events/ApplicationEvent.h"
#include <JuicyEngine/Events/KeyEvent.h>
#include <JuicyEngine/Events/MouseEvent.h>
namespace JuicyEngine
{
class JE_API ImGuiLayer : public Layer
{
public:
    ImGuiLayer();
    ~ImGuiLayer();
    virtual void OnAttach() override;
    virtual void OnDetach() override;
    virtual void OnImGuiRender() override;
    void Begin();
    void End();
};
}  // namespace JuicyEngine