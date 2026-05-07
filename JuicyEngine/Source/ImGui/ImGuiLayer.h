#pragma once

#include "Core/Layer.h"
#include "vulkan/vulkan.h"

namespace JuicyEngine
{
class ImGuiLayer : public Layer
{
public:
    ImGuiLayer();
    ~ImGuiLayer();

    virtual void OnAttach();
    virtual void OnDetach();
    virtual void OnUpdate();
    virtual void OnEvent(Event& event);

    void Begin();
    void Render();

    static ImGuiLayer* Get()
    {
        return s_Instance;
    }

private:
    static ImGuiLayer* s_Instance;
    VkDescriptorPool ImGuiDescriptorPool;
};
}
