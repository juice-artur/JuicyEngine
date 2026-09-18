#include "VulkanSurface.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

#include "Core/Core.h"

namespace JuicyEngine
{
VulkanSurface::VulkanSurface(const VkInstance& Instance) : Instance(Instance) {}

void VulkanSurface::Init(void* Window)
{
    SDL_Window* SDLWindow = static_cast<SDL_Window*>(Window);
    JE_CORE_ASSERT(SDLWindow, "Window handle is null!")

    if (!SDL_Vulkan_CreateSurface(SDLWindow, Instance, nullptr, &Surface))
    {
        JE_CORE_ASSERT(false, "Failed to create Vulkan surface: {0}", SDL_GetError())
    }
}

void VulkanSurface::Shutdown()
{
    vkDestroySurfaceKHR(Instance, Surface, nullptr);
}

const VkSurfaceKHR& VulkanSurface::GetSurface()
{
    return Surface;
}
} // namespace JuicyEngine