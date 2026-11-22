#include "VulkanBuffer.h"

#include "VulkanContext.h"
#include "VulkanUtils.h"
#include "Core/Core.h"

namespace JuicyEngine
{
VkBuffer& VulkanBuffer::GetBuffer()
{
    return Buffer;
}

VulkanBuffer::~VulkanBuffer()
{
    vkDestroyBuffer(m_Context.GetDevice()->GetLogicalDevice(), Buffer, nullptr);
    vkFreeMemory(m_Context.GetDevice()->GetLogicalDevice(), BufferMemory, nullptr);
}

void VulkanBuffer::CopyBuffer(VkBuffer SrcBuffer, VkBuffer DstBuffer, VkDeviceSize Size)
{
    VkCommandBuffer CommandBuffer = RHI::Vulkan::VulkanUtils::BeginSingleTimeCommands(m_Context);

    VkBufferCopy CopyRegion {};
    CopyRegion.size = Size;
    vkCmdCopyBuffer(CommandBuffer, SrcBuffer, DstBuffer, 1, &CopyRegion);

    RHI::Vulkan::VulkanUtils::EndSingleTimeCommands(m_Context, CommandBuffer);
}

VulkanVertexBuffer::VulkanVertexBuffer(VulkanContext& Context, const std::vector<Vertex>& Vertexes)
    : VulkanBuffer(Context)
{
    const auto VertexCount = Vertexes.size();
    JE_CORE_ASSERT(VertexCount > 0, "Vertex buffer creation requires at least one vertex")

    const VkDeviceSize BufferSize = VertexCount * sizeof(Vertex);

    VkBuffer StagingBuffer;
    VkDeviceMemory StagingBufferMemory;
    RHI::Vulkan::VulkanUtils::CreateBuffer(m_Context,
                                           BufferSize,
                                           VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                           StagingBuffer,
                                           StagingBufferMemory);

    void* Data;
    const VkResult MapResult =
        vkMapMemory(m_Context.GetDevice()->GetLogicalDevice(), StagingBufferMemory, 0, BufferSize, 0, &Data);
    JE_CORE_ASSERT(MapResult == VK_SUCCESS, "Failed to map staging buffer memory")

    memcpy(Data, Vertexes.data(), (size_t) BufferSize);
    vkUnmapMemory(m_Context.GetDevice()->GetLogicalDevice(), StagingBufferMemory);

    RHI::Vulkan::VulkanUtils::CreateBuffer(m_Context,
                                           BufferSize,
                                           VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                                           VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                           Buffer,
                                           BufferMemory);

    CopyBuffer(StagingBuffer, Buffer, BufferSize);

    vkDestroyBuffer(m_Context.GetDevice()->GetLogicalDevice(), StagingBuffer, nullptr);
    vkFreeMemory(m_Context.GetDevice()->GetLogicalDevice(), StagingBufferMemory, nullptr);
}

VulkanVertexBuffer::~VulkanVertexBuffer() {}

VulkanIndexBuffer::VulkanIndexBuffer(VulkanContext& Context, const std::vector<uint16_t>& Indexes)
    : VulkanBuffer(Context)
{
    Init(Indexes.data(), Indexes.size(), sizeof(uint16_t));
}
VulkanIndexBuffer::VulkanIndexBuffer(VulkanContext& Context, const std::vector<uint32_t>& Indexes)
    : VulkanBuffer(Context)
{
    Init(Indexes.data(), Indexes.size(), sizeof(uint32_t));
}

VulkanIndexBuffer::~VulkanIndexBuffer() {}

void VulkanIndexBuffer::Init(const void* Source, size_t Count, size_t ElementSize)
{
    JE_CORE_ASSERT(Count > 0, "Index buffer creation requires at least one index")

    const VkDeviceSize BufferSize = static_cast<VkDeviceSize>(Count) * ElementSize;

    VkBuffer StagingBuffer;
    VkDeviceMemory StagingBufferMemory;
    RHI::Vulkan::VulkanUtils::CreateBuffer(m_Context,
                                           BufferSize,
                                           VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                           StagingBuffer,
                                           StagingBufferMemory);

    void* Data = nullptr;
    vkMapMemory(m_Context.GetDevice()->GetLogicalDevice(), StagingBufferMemory, 0, BufferSize, 0, &Data);
    memcpy(Data, Source, static_cast<size_t>(BufferSize));
    vkUnmapMemory(m_Context.GetDevice()->GetLogicalDevice(), StagingBufferMemory);

    RHI::Vulkan::VulkanUtils::CreateBuffer(m_Context,
                                           BufferSize,
                                           VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                                           VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                                           Buffer,
                                           BufferMemory);

    CopyBuffer(StagingBuffer, Buffer, BufferSize);

    vkDestroyBuffer(m_Context.GetDevice()->GetLogicalDevice(), StagingBuffer, nullptr);
    vkFreeMemory(m_Context.GetDevice()->GetLogicalDevice(), StagingBufferMemory, nullptr);
}

VulkanUniformBuffer::VulkanUniformBuffer(VulkanContext& Context, int Size)
    : VulkanBuffer(Context)
{
    RHI::Vulkan::VulkanUtils::CreateBuffer(m_Context,
                                           Size,
                                           VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                                           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                                           Buffer,
                                           BufferMemory);
}

VulkanUniformBuffer::~VulkanUniformBuffer() {}

void VulkanUniformBuffer::UploadData(int Size, void* Data)
{
    void* MappedData = nullptr;
    VkResult MapResult =
        vkMapMemory(m_Context.GetDevice()->GetLogicalDevice(), BufferMemory, 0, Size, 0, &MappedData);
    JE_CORE_ASSERT(MapResult == VK_SUCCESS, "Failed to map uniform buffer memory")

    memcpy(MappedData, Data, Size);
    vkUnmapMemory(m_Context.GetDevice()->GetLogicalDevice(), BufferMemory);
}
} // namespace JuicyEngine
