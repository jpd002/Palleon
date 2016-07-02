#pragma once

#include "palleon/graphics/Texture.h"
#include "vulkan/Device.h"
#include "vulkan/CommandBufferPool.h"

namespace Palleon
{
	class CVulkanTexture : public CTexture
	{
	public:
		        CVulkanTexture() = default;
		        CVulkanTexture(Framework::Vulkan::CDevice&, const VkPhysicalDeviceMemoryProperties&, VkQueue, Framework::Vulkan::CCommandBufferPool&, TEXTURE_FORMAT, uint32, uint32);
		virtual ~CVulkanTexture();

		void*   GetHandle() const override;
		
		void    Update(uint32, const void*) override;
		void    UpdateCubeFace(TEXTURE_CUBE_FACE, const void*) override;
		
	private:
		Framework::Vulkan::CDevice*             m_device = nullptr;
		const VkPhysicalDeviceMemoryProperties* m_memoryProperties = nullptr;
		VkQueue                                 m_queue = VK_NULL_HANDLE;
		Framework::Vulkan::CCommandBufferPool*  m_commandBufferPool = nullptr;
		
		VkImage           m_imageHandle = VK_NULL_HANDLE;
		VkDeviceMemory    m_imageMemoryHandle = VK_NULL_HANDLE;
		VkImageView       m_imageViewHandle = VK_NULL_HANDLE;
	};
}
