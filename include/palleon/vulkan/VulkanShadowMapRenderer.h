#pragma once

#include <vector>
#include "vulkan/Device.h"
#include "VulkanShadowMapEffect.h"

namespace Palleon
{
	class CViewport;
	class CMesh;
	
	class CVulkanShadowMapRenderer
	{
	public:
		CVulkanShadowMapRenderer(Framework::Vulkan::CDevice&, const VkPhysicalDeviceMemoryProperties&);
		virtual ~CVulkanShadowMapRenderer();
		
		VkImageView GetImageView() const;
		
		void    DrawViewport(VkCommandBuffer, CViewport*);
		
	private:
		typedef std::vector<CMesh*> RenderQueue;
		
		VkRenderPass    CreateRenderPass(VkFormat, VkFormat, bool);
		void            CreateMap(const VkPhysicalDeviceMemoryProperties&);
		void            CreateDepth(const VkPhysicalDeviceMemoryProperties&);
		void            CreateFramebuffer();
		
		Framework::Vulkan::CDevice*    m_device = nullptr;
		
		VkRenderPass      m_shadowMapRenderPass = VK_NULL_HANDLE;

		VkImage           m_shadowMapImage = VK_NULL_HANDLE;
		VkDeviceMemory    m_shadowMapMemory = VK_NULL_HANDLE;
		VkImageView       m_shadowMapImageView = VK_NULL_HANDLE;

		VkImage           m_shadowMapDepthImage = VK_NULL_HANDLE;
		VkDeviceMemory    m_shadowMapDepthMemory = VK_NULL_HANDLE;
		VkImageView       m_shadowMapDepthImageView = VK_NULL_HANDLE;

		VkFramebuffer     m_shadowMapFramebuffer = VK_NULL_HANDLE;
		
		CVulkanShadowMapEffect m_effect;
	};
}
