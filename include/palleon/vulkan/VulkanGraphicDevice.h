#pragma once

#include "vulkan/Instance.h"
#include "vulkan/Device.h"
#include "vulkan/CommandBufferPool.h"
#include "palleon/graphics/GraphicDevice.h"
#include "palleon/graphics/Mesh.h"

namespace Palleon
{
	class CVulkanGraphicDevice : public CGraphicDevice
	{
	public:
		void                   Draw() override;
		
		VertexBufferPtr        CreateVertexBuffer(const VERTEX_BUFFER_DESCRIPTOR&) override;
		
		TexturePtr             CreateTexture(TEXTURE_FORMAT, uint32, uint32, uint32) override;
		TexturePtr             CreateCubeTexture(TEXTURE_FORMAT, uint32) override;
		
		RenderTargetPtr        CreateRenderTarget(TEXTURE_FORMAT, uint32, uint32) override;
		CubeRenderTargetPtr    CreateCubeRenderTarget(TEXTURE_FORMAT, uint32) override;
		
		SharedGraphicContextPtr CreateSharedContext() override;
		
	protected:
		typedef std::vector<CMesh*> RenderQueue;
		
		           CVulkanGraphicDevice(const CVector2&, float);
		virtual    ~CVulkanGraphicDevice();
		
		void       Initialize();
		
		void       CreateInstance();
		void       CreateDebugReportCallback();
		void       CreateDevice(VkPhysicalDevice);
		
		VkRenderPass    CreateRenderPass(VkFormat, VkFormat, bool);
		
		void       CreateDepthbuffer(VkExtent2D, VkFormat);

		void       CreateSwapChain(VkSurfaceFormatKHR, VkExtent2D);
		void       PrepareSwapChainImages();
		void       CreateSwapChainImageViews(VkFormat);
		void       CreateSwapChainFramebuffers(VkRenderPass, VkExtent2D);
		
		std::vector<VkPhysicalDevice>   GetPhysicalDevices();
		std::vector<uint32_t>           GetRenderQueueFamilies(VkPhysicalDevice);
		std::vector<VkSurfaceFormatKHR> GetDeviceSurfaceFormats(VkPhysicalDevice);
		
		void                            DrawViewport(VkCommandBuffer, CViewport*, VkRenderPass, VkFramebuffer, VkExtent2D);
		
		Framework::Vulkan::CInstance     m_vkInstance;
		VkDebugReportCallbackEXT         m_debugReportCallback = VK_NULL_HANDLE;
		VkSurfaceKHR                     m_surface = VK_NULL_HANDLE;
		VkExtent2D                       m_surfaceExtents;
		Framework::Vulkan::CDevice       m_device;
		Framework::Vulkan::CCommandBufferPool m_commandBufferPool;
		VkDescriptorPool                 m_descriptorPool = VK_NULL_HANDLE;

		VkPhysicalDeviceMemoryProperties m_physicalDeviceMemoryProperties;
		VkQueue                          m_queue = VK_NULL_HANDLE;
		VkRenderPass                     m_mainRenderPass = VK_NULL_HANDLE;
		VkRenderPass                     m_additionalRenderPass = VK_NULL_HANDLE;
		VkSemaphore                      m_imageAcquireSemaphore = VK_NULL_HANDLE;
		VkSemaphore                      m_renderCompleteSemaphore = VK_NULL_HANDLE;
		VkSwapchainKHR                   m_swapChain = VK_NULL_HANDLE;
		std::vector<VkImage>             m_swapChainImages;
		std::vector<VkImageView>         m_swapChainImageViews;
		std::vector<VkFramebuffer>       m_swapChainFramebuffers;
		VkImage                          m_depthbufferImage = VK_NULL_HANDLE;
		VkDeviceMemory                   m_depthbufferMemory = VK_NULL_HANDLE;
		VkImageView                      m_depthbufferImageView = VK_NULL_HANDLE;
		VkSampler                        m_genericSampler = VK_NULL_HANDLE;
	};
}
