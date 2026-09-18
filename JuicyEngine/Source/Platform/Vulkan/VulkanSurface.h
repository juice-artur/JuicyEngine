#pragma once

#include <vulkan/vulkan.h>

struct SDL_Window;

namespace JuicyEngine
{
class VulkanSurface
{
public:
    VulkanSurface(const VkInstance& Instance);

    void Init(void* Window);
    void Shutdown();
    const VkSurfaceKHR& GetSurface();

private:
    VkSurfaceKHR Surface;

    const VkInstance& Instance;
};
} // namespace JuicyEngine