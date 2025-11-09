#include "GraphicsContext.h"

#include "Platform/Vulkan/VulkanContext.h"

namespace JuicyEngine
{
	GraphicsContext* GraphicsContext::Get()
	{
		static VulkanContext Instance;
		return &Instance;
	}
} // namespace JuicyEngine