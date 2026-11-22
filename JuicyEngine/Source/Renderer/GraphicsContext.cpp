#include "GraphicsContext.h"

#include "Platform/Vulkan/VulkanContext.h"

namespace JuicyEngine
{
std::unique_ptr<GraphicsContext> GraphicsContext::Create(RendererAPI API)
{
    switch (API)
    {
    case RendererAPI::Vulkan:
        return std::make_unique<VulkanContext>();
    }
    return nullptr;
}
} // namespace JuicyEngine
