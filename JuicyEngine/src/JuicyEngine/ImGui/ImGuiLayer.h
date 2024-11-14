#pragma once
#include "JuicyEngine/Core/Layer.h"
#include "JuicyEngine/Events/ApplicationEvent.h"
#include <JuicyEngine/Events/KeyEvent.h>
#include <JuicyEngine/Events/MouseEvent.h>
namespace JuicyEngine
{
class  ImGuiLayer : public Layer
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