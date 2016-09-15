#include "palleon/vulkan/VulkanVertexBuffer.h"
#include "palleon/vulkan/VulkanUtils.h"
#include "vulkan/StructDefs.h"

using namespace Palleon;

CVulkanVertexBuffer::CVulkanVertexBuffer(Framework::Vulkan::CDevice& device, const VkPhysicalDeviceMemoryProperties& memoryProperties, const VERTEX_BUFFER_DESCRIPTOR& descriptor)
: CVertexBuffer(descriptor)
, m_device(&device)
{
	VkResult result = VK_SUCCESS;
	
	{
		auto bufferCreateInfo = Framework::Vulkan::BufferCreateInfo();
		bufferCreateInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		bufferCreateInfo.size  = descriptor.GetVertexBufferSize();
		result = m_device->vkCreateBuffer(*m_device, &bufferCreateInfo, nullptr, &m_vertexBufferHandle);
		CHECKVULKANERROR(result);
	}
	
	{
		auto bufferCreateInfo = Framework::Vulkan::BufferCreateInfo();
		bufferCreateInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
		bufferCreateInfo.size  = sizeof(uint16) * descriptor.indexCount;
		result = m_device->vkCreateBuffer(*m_device, &bufferCreateInfo, nullptr, &m_indexBufferHandle);
		CHECKVULKANERROR(result);
	}
	
	m_vertexBufferMemoryHandle = AllocateAndBindBufferMemory(m_vertexBufferHandle, memoryProperties);
	m_indexBufferMemoryHandle = AllocateAndBindBufferMemory(m_indexBufferHandle, memoryProperties);
}

CVulkanVertexBuffer::~CVulkanVertexBuffer()
{
	m_device->vkFreeMemory(*m_device, m_vertexBufferMemoryHandle, nullptr);
	m_device->vkFreeMemory(*m_device, m_indexBufferMemoryHandle, nullptr);
	
	m_device->vkDestroyBuffer(*m_device, m_vertexBufferHandle, nullptr);
	m_device->vkDestroyBuffer(*m_device, m_indexBufferHandle, nullptr);
}

void CVulkanVertexBuffer::UnlockVertices(uint32)
{
	VkResult result = VK_SUCCESS;
	void* bufferMemoryData = nullptr;
	result = m_device->vkMapMemory(*m_device, m_vertexBufferMemoryHandle, 0, VK_WHOLE_SIZE, 0, &bufferMemoryData);
	CHECKVULKANERROR(result);
	memcpy(bufferMemoryData, m_shadowVertexBuffer, m_descriptor.GetVertexBufferSize());
	m_device->vkUnmapMemory(*m_device, m_vertexBufferMemoryHandle);
}

void CVulkanVertexBuffer::UnlockIndices()
{
	VkResult result = VK_SUCCESS;
	void* bufferMemoryData = nullptr;
	result = m_device->vkMapMemory(*m_device, m_indexBufferMemoryHandle, 0, VK_WHOLE_SIZE, 0, &bufferMemoryData);
	CHECKVULKANERROR(result);
	memcpy(bufferMemoryData, m_shadowIndexBuffer, sizeof(uint16) * m_descriptor.indexCount);
	m_device->vkUnmapMemory(*m_device, m_indexBufferMemoryHandle);
}

VkBuffer CVulkanVertexBuffer::GetVertexBuffer() const
{
	return m_vertexBufferHandle;
}

VkBuffer CVulkanVertexBuffer::GetIndexBuffer() const
{
	return m_indexBufferHandle;
}

VkDeviceMemory CVulkanVertexBuffer::AllocateAndBindBufferMemory(VkBuffer bufferHandle, const VkPhysicalDeviceMemoryProperties& memoryProperties)
{
	VkResult result = VK_SUCCESS;
	
	VkDeviceMemory bufferMemoryHandle = VK_NULL_HANDLE;
	
	VkMemoryRequirements memoryRequirements = {};
	m_device->vkGetBufferMemoryRequirements(*m_device, bufferHandle, &memoryRequirements);

	auto memoryAllocateInfo = Framework::Vulkan::MemoryAllocateInfo();
	memoryAllocateInfo.allocationSize = memoryRequirements.size;
	memoryAllocateInfo.memoryTypeIndex = GetMemoryTypeIndex(memoryProperties, memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	assert(memoryAllocateInfo.memoryTypeIndex != VULKAN_MEMORY_TYPE_INVALID);

	result = m_device->vkAllocateMemory(*m_device, &memoryAllocateInfo, nullptr, &bufferMemoryHandle);
	CHECKVULKANERROR(result);
	
	result = m_device->vkBindBufferMemory(*m_device, bufferHandle, bufferMemoryHandle, 0);
	CHECKVULKANERROR(result);
	
	return bufferMemoryHandle;
}
