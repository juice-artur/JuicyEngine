#pragma once

#include "vulkan/vulkan.h"

namespace JuicyEngine
{
class VulkanRenderCommandBuffer
{
public:
   void Init(VkDevice Device, VkCommandPool CommandPool);

    void Begin();

    void End();

    VkCommandBuffer& GetCommandBuffer();
    
private:
    VkCommandBuffer CommandBuffer = VK_NULL_HANDLE;
};


}