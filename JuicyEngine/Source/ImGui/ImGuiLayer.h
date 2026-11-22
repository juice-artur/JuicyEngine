#pragma once

#include "Core/Layer.h"
#include "vulkan/vulkan.h"

namespace JuicyEngine
{
class Window;
class VulkanContext;

class ImGuiLayer : public Layer
{
public:
    ImGuiLayer(Window& Window, VulkanContext& Context);
    ~ImGuiLayer() override;

    void OnAttach() override;
    void OnDetach() override;
    void OnUpdate() override;
    void OnEvent(Event& event) override;

    void Begin();
    void Render();

private:
    Window& m_Window;
    VulkanContext& m_Context;
    VkDescriptorPool ImGuiDescriptorPool = VK_NULL_HANDLE;
};
} // namespace JuicyEngine
