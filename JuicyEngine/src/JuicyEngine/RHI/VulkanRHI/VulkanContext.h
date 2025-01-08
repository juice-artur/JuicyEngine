#pragma once

#include "JuicyEngine/Renderer/GraphicsContext.h"
#include "JuicyEngine/Core/Window.h"
#include "vulkan/vulkan.h"
#include "VulkanDevice.h"

namespace JuicyEngine
{
class VulkanContext : public GraphicsContext
{
public:
    VulkanContext(Window* windowHandle);
    virtual ~VulkanContext() override;
    // Inherited via GraphicsContext
    void Init() override;
    void SwapBuffers() override;

private:
    void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
    void SetupDebugMessenger();

private:
    Window* m_Window;

    VkInstance m_Instance;
    VkDebugUtilsMessengerEXT debugMessenger;

    VulkanDevice m_Device;
};
}  // namespace JuicyEngine