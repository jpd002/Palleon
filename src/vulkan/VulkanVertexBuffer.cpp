#include "palleon/vulkan/VulkanVertexBuffer.h"
#include "vulkan/StructDefs.h"

using namespace Palleon;

static uint32 GetMemoryTypeIndex(const VkPhysicalDeviceMemoryProperties& memoryProperties, uint32 deviceRequirements, uint32 hostRequirements)
{
	for(uint32 i = 0; i < VK_MAX_MEMORY_TYPES; i++)
	{
		if(deviceRequirements & (1 << i))
		{
			const auto& memoryType = memoryProperties.memoryTypes[i];
			if((memoryType.propertyFlags & hostRequirements) == hostRequirements)
			{
				return i;
			}
		}
	}
	assert(false);
	return 0;
}

CVulkanVertexBuffer::CVulkanVertexBuffer(Framework::Vulkan::CDevice& device, const VkPhysicalDeviceMemoryProperties& memoryProperties, const VERTEX_BUFFER_DESCRIPTOR& descriptor)
: CVertexBuffer(descriptor)
, m_device(&device)
{
	VkResult result = VK_SUCCESS;
	
	uint32 vertexSize = descriptor.GetVertexSize();
	
	auto bufferCreateInfo = Framework::Vulkan::BufferCreateInfo();
	bufferCreateInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	bufferCreateInfo.size  = vertexSize * descriptor.vertexCount;
	result = m_device->vkCreateBuffer(*m_device, &bufferCreateInfo, nullptr, &m_bufferHandle);
	CHECKVULKANERROR(result);
	
	VkMemoryRequirements memoryRequirements = {};
	m_device->vkGetBufferMemoryRequirements(*m_device, m_bufferHandle, &memoryRequirements);

	auto memoryAllocateInfo = Framework::Vulkan::MemoryAllocateInfo();
	memoryAllocateInfo.allocationSize = memoryRequirements.size;
	memoryAllocateInfo.memoryTypeIndex = GetMemoryTypeIndex(memoryProperties, memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	result = m_device->vkAllocateMemory(*m_device, &memoryAllocateInfo, nullptr, &m_bufferMemoryHandle);
	CHECKVULKANERROR(result);
	
	result = m_device->vkBindBufferMemory(*m_device, m_bufferHandle, m_bufferMemoryHandle, 0);
	CHECKVULKANERROR(result);
}

CVulkanVertexBuffer::~CVulkanVertexBuffer()
{
	m_device->vkFreeMemory(*m_device, m_bufferMemoryHandle, nullptr);
	m_device->vkDestroyBuffer(*m_device, m_bufferHandle, nullptr);
}

void CVulkanVertexBuffer::UnlockVertices(uint32)
{
	VkResult result = VK_SUCCESS;
	void* bufferMemoryData = nullptr;
	result = m_device->vkMapMemory(*m_device, m_bufferMemoryHandle, 0, VK_WHOLE_SIZE, 0, &bufferMemoryData);
	CHECKVULKANERROR(result);
	memcpy(bufferMemoryData, m_shadowVertexBuffer, m_descriptor.GetVertexBufferSize());
	m_device->vkUnmapMemory(*m_device, m_bufferMemoryHandle);
}

void CVulkanVertexBuffer::UnlockIndices()
{
	
}

VkBuffer CVulkanVertexBuffer::GetVertexBuffer() const
{
	return m_bufferHandle;
}
