#pragma once

#include "palleon/graphics/VertexBuffer.h"
#include "vulkan/Device.h"

namespace Palleon
{
	class CVulkanVertexBuffer : public CVertexBuffer
	{
	public:
		        CVulkanVertexBuffer(Framework::Vulkan::CDevice&, const VkPhysicalDeviceMemoryProperties&, const VERTEX_BUFFER_DESCRIPTOR&);
		virtual ~CVulkanVertexBuffer();
		
		void    UnlockVertices(uint32) override;
		void    UnlockIndices() override;
		
		VkBuffer GetVertexBuffer() const;
		VkBuffer GetIndexBuffer() const;
		
	private:
		VkDeviceMemory                 AllocateAndBindBufferMemory(VkBuffer, const VkPhysicalDeviceMemoryProperties&);
		
		Framework::Vulkan::CDevice*    m_device = nullptr;
		
		VkBuffer                       m_vertexBufferHandle = VK_NULL_HANDLE;
		VkBuffer                       m_indexBufferHandle = VK_NULL_HANDLE;
		
		VkDeviceMemory                 m_vertexBufferMemoryHandle = VK_NULL_HANDLE;
		VkDeviceMemory                 m_indexBufferMemoryHandle = VK_NULL_HANDLE;
	};
}
