#include "VulkanMesh.h"

#include "VulkanMesh.h"

#include "Platform/Vulkan/VulkanBuffer.h"
#include "Platform/Vulkan/VulkanContext.h"
#include "Core/Core.h"

namespace JuicyEngine
{
VulkanMesh::VulkanMesh(VulkanContext& Context,
                       const Mesh& Data,
                       VkDescriptorPool Pool,
                       VkDescriptorSetLayout Layout,
                       VkBuffer UniformBuffer,
                       VkDeviceSize UniformBufferSize)
    : m_Context(Context), m_Pool(Pool), m_IndexCount(static_cast<uint32_t>(Data.Indices.size()))
{
    const VkDevice Device = m_Context.GetDevice()->GetLogicalDevice();

    JE_CORE_ASSERT(!Data.Vertices.empty() && !Data.Indices.empty(), "Cannot create GPU mesh from empty data")

    m_VertexBuffer = std::make_unique<VulkanVertexBuffer>(m_Context, Data.Vertices);
    m_IndexBuffer = std::make_unique<VulkanIndexBuffer>(m_Context, Data.Indices);

    if (Data.BaseColorTexture.Valid())
    {
        m_Texture = std::make_unique<VulkanTexture2D>(m_Context,
                                                       Data.BaseColorTexture.Pixels.data(),
                                                      Data.BaseColorTexture.Width,
                                                      Data.BaseColorTexture.Height);
    }
    else
    {
        // fallback: 1x1 white texture
        constexpr uint8_t WhitePixel[4] = {255, 255, 255, 255};
        m_Texture = std::make_unique<VulkanTexture2D>(m_Context, WhitePixel, 1, 1);
    }

    VkDescriptorSetAllocateInfo AllocateInfo {};
    AllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    AllocateInfo.descriptorPool = m_Pool;
    AllocateInfo.descriptorSetCount = 1;
    AllocateInfo.pSetLayouts = &Layout;

    VkResult Result = vkAllocateDescriptorSets(Device, &AllocateInfo, &m_DescriptorSet);
    JE_CORE_ASSERT(Result == VK_SUCCESS, "Failed to allocate mesh descriptor set!")

    VkDescriptorBufferInfo BufferInfo {UniformBuffer, 0, UniformBufferSize};
    VkDescriptorImageInfo ImageInfo {};
    ImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    ImageInfo.imageView = m_Texture->GetImage()->GetImageView();
    ImageInfo.sampler = m_Texture->GetSampler();

    VkWriteDescriptorSet Writes[2] {};
    Writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    Writes[0].dstSet = m_DescriptorSet;
    Writes[0].dstBinding = 0;
    Writes[0].descriptorCount = 1;
    Writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    Writes[0].pBufferInfo = &BufferInfo;

    Writes[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    Writes[1].dstSet = m_DescriptorSet;
    Writes[1].dstBinding = 1;
    Writes[1].descriptorCount = 1;
    Writes[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    Writes[1].pImageInfo = &ImageInfo;

    vkUpdateDescriptorSets(Device, 2, Writes, 0, nullptr);
}

VulkanMesh::~VulkanMesh()
{
    if (m_DescriptorSet != VK_NULL_HANDLE && m_Pool != VK_NULL_HANDLE)
        vkFreeDescriptorSets(m_Context.GetDevice()->GetLogicalDevice(), m_Pool, 1, &m_DescriptorSet);
}
} // namespace JuicyEngine