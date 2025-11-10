#pragma once

#include "Renderer/Buffer.h"
#include "vulkan/vulkan.h"

/**
 * Selects a device memory type index that satisfies the given type filter and memory property flags.
 * @param TypeFilter Bitmask of acceptable memory types from Vulkan memory requirements.
 * @param Properties Required memory property flags.
 * @returns The index of a memory type that meets the filter and properties.
 */
/**
 * Returns the underlying Vulkan buffer handle.
 * @returns A reference to the internal VkBuffer handle.
 */
/**
 * Virtual destructor that releases Vulkan buffer resources held by this object.
 */
 /**
  * Creates a Vulkan buffer and allocates memory for it, assigning the results to the provided handles.
  * @param Size Size of the buffer in bytes.
  * @param Usage Buffer usage flags.
  * @param Properties Memory property flags required for the allocation.
  * @param InBuffer Output parameter that will receive the created VkBuffer handle.
  * @param InBufferMemory Output parameter that will receive the allocated VkDeviceMemory handle.
  */
/**
 * Copies `Size` bytes of data from a source Vulkan buffer to a destination Vulkan buffer.
 * @param SrcBuffer Source VkBuffer to copy from.
 * @param DstBuffer Destination VkBuffer to copy to.
 * @param Size Number of bytes to copy.
 */
/**
 * Constructs a Vulkan vertex buffer and uploads the provided vertex data into GPU memory.
 * @param Vertexes The vertex data used to initialize the buffer.
 */
/**
 * Destructor that releases resources owned by the VulkanVertexBuffer.
 */
/**
 * Constructs a Vulkan index buffer and uploads the provided index data into GPU memory.
 * @param Indexes The index data used to initialize the buffer.
 */
/**
 * Destructor that releases resources owned by the VulkanIndexBuffer.
 */
namespace JuicyEngine
{
	class VulkanBuffer
	{
	public:
		uint32_t FindMemoryType(uint32_t TypeFilter, VkMemoryPropertyFlags Properties);
		VkBuffer& GetBuffer();
		virtual ~VulkanBuffer();

	protected:
		void CreateBuffer(VkDeviceSize Size, VkBufferUsageFlags Usage, VkMemoryPropertyFlags Properties, VkBuffer& InBuffer, VkDeviceMemory& InBufferMemory);
		void CopyBuffer(VkBuffer SrcBuffer, VkBuffer DstBuffer, VkDeviceSize Size);
		
	protected:
		VkBuffer Buffer = VK_NULL_HANDLE;
		VkDeviceMemory BufferMemory = VK_NULL_HANDLE;
	};

	class VulkanVertexBuffer : public VertexBuffer, public VulkanBuffer
	{
	public:
		VulkanVertexBuffer(const std::vector<Vertex>& Vertexes);

		~VulkanVertexBuffer() override;
	};

	class VulkanIndexBuffer : public VertexBuffer, public VulkanBuffer
	{
	public:
		VulkanIndexBuffer(const std::vector<uint16_t>& Indexes);

		~VulkanIndexBuffer() override;
	};
} // namespace JuicyEngine