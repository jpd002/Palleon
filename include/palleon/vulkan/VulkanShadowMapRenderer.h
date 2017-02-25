#pragma once

#include "vulkan/Device.h"

namespace Palleon
{
	class CVulkanShadowMapRenderer
	{
	public:
		CVulkanShadowMapRenderer(Framework::Vulkan::CDevice&, const VkPhysicalDeviceMemoryProperties&);
		virtual ~CVulkanShadowMapRenderer();
		
	private:
		VkRenderPass    CreateRenderPass(VkFormat, VkFormat, bool);
		void            CreateMap(const VkPhysicalDeviceMemoryProperties&);
		
		Framework::Vulkan::CDevice*    m_device = nullptr;
		
		VkRenderPass      m_shadowMapRenderPass = VK_NULL_HANDLE;
		VkImage           m_shadowMapImage = VK_NULL_HANDLE;
		VkDeviceMemory    m_shadowMapMemory = VK_NULL_HANDLE;
		VkImageView       m_shadowMapImageView = VK_NULL_HANDLE;
		VkFramebuffer     m_shadowMapFramebuffer = VK_NULL_HANDLE;
	};
}
