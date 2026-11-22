#pragma once

#include "Renderer/Mesh.h"
#include "Platform/Vulkan/VulkanTexture2D.h"
#include "vulkan/vulkan.h"

namespace JuicyEngine
{
class VulkanContext;

class VulkanMesh
{
public:
    VulkanMesh(VulkanContext& Context,
               const Mesh& Data,
               VkDescriptorPool Pool,
               VkDescriptorSetLayout Layout,
               VkBuffer UniformBuffer,
               VkDeviceSize UniformBufferSize);
    ~VulkanMesh();

    VkBuffer GetVertexBuffer() const { return m_VertexBuffer->GetBuffer(); }
    VkBuffer GetIndexBuffer() const { return m_IndexBuffer->GetBuffer(); }
    VkIndexType GetIndexType() const { return VK_INDEX_TYPE_UINT32; }
    uint32_t GetIndexCount() const { return m_IndexCount; }
    VkDescriptorSet GetDescriptorSet() const { return m_DescriptorSet; }

    VulkanMesh(const VulkanMesh&) = delete;
    VulkanMesh& operator=(const VulkanMesh&) = delete;

private:
    VulkanContext& m_Context;
    std::unique_ptr<VulkanVertexBuffer> m_VertexBuffer;
    std::unique_ptr<VulkanIndexBuffer> m_IndexBuffer;
    std::unique_ptr<VulkanTexture2D> m_Texture;
    VkDescriptorPool m_Pool = VK_NULL_HANDLE;
    VkDescriptorSet m_DescriptorSet = VK_NULL_HANDLE;
    uint32_t m_IndexCount = 0;
};
} // namespace JuicyEngine