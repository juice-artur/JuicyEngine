#pragma once

#include "Renderer/Buffer.h"
#include "vulkan/vulkan.h"

namespace JuicyEngine
{
class VulkanContext;

class VulkanBuffer
{
public:
    VkBuffer& GetBuffer();
    virtual ~VulkanBuffer();

protected:
    explicit VulkanBuffer(VulkanContext& Context) : m_Context(Context) {}
    void CopyBuffer(VkBuffer SrcBuffer, VkBuffer DstBuffer, VkDeviceSize Size);

protected:
    VulkanContext& m_Context;
    VkBuffer Buffer = VK_NULL_HANDLE;
    VkDeviceMemory BufferMemory = VK_NULL_HANDLE;
};

class VulkanVertexBuffer : public VertexBuffer, public VulkanBuffer
{
public:
    VulkanVertexBuffer(VulkanContext& Context, const std::vector<Vertex>& Vertexes);
    ~VulkanVertexBuffer() override;
};

class VulkanIndexBuffer : public IndexBuffer, public VulkanBuffer
{
public:
    VulkanIndexBuffer(VulkanContext& Context, const std::vector<uint16_t>& Indexes);
    VulkanIndexBuffer(VulkanContext& Context, const std::vector<uint32_t>& Indexes);
    ~VulkanIndexBuffer() override;

private:
    void Init(const void* Source, size_t Count, size_t ElementSize);
};

class VulkanUniformBuffer : public UniformBuffer, public VulkanBuffer
{
public:
    VulkanUniformBuffer(VulkanContext& Context, int Size);
    ~VulkanUniformBuffer() override;
    void UploadData(int Size, void* Data) override;
};
} // namespace JuicyEngine
