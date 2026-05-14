#pragma once

#include "VulkanBuffer.h"
#include "VulkanImage.h"
#include "vulkan/vulkan.h"

namespace JuicyEngine
{
class VulkanTexture2D
{
public:
    VulkanTexture2D(const std::string& Path);
    ~VulkanTexture2D();
    VkSampler& GetSampler();
    VulkanImage* GetImage();

    VulkanTexture2D(const VulkanTexture2D&) = delete;
    VulkanTexture2D& operator=(const VulkanTexture2D&) = delete;
    VulkanTexture2D(VulkanTexture2D&&) = delete;
    VulkanTexture2D& operator=(VulkanTexture2D&&) = delete;

protected:
    void CreateTextureSampler();
    void UploadData(VkBuffer Buffer);

private:
    int32_t Width;
    int32_t Height;
    std::unique_ptr<VulkanImage> Image;
    VkSampler TextureSampler;
};
}