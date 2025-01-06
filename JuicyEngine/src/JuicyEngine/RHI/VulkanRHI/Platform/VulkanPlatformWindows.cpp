#include "JuicyEngine/RHI/VulkanRHI/Platform/VulkanPlatform.h"

#ifdef JE_PLATFORM_WINDOWS

namespace JuicyEngine
{
void GetVulkanPlatformRequiredExtensionNames(std::vector<const char*>& names)
{
    names.push_back("VK_KHR_win32_surface");
}
}  // namespace JuicyEngine
#endif