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
		
	private:
		Framework::Vulkan::CDevice*    m_device = nullptr;
		VkBuffer                       m_bufferHandle = VK_NULL_HANDLE;
		VkDeviceMemory                 m_bufferMemoryHandle = VK_NULL_HANDLE;
	};
}
