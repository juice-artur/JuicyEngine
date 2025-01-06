#include "jepch.h"
#include "VulkanContext.h"

namespace JuicyEngine
{
JuicyEngine::VulkanContext::VulkanContext(Window* windowHandle) : m_Window(windowHandle)
{
    JE_CORE_ASSERT(windowHandle, "Window handle is null!");
}
void VulkanContext::Init() {}
void VulkanContext::SwapBuffers()
{
    JE_CORE_ASSERT(true, "Implement this!");
}
}  // namespace JuicyEngine