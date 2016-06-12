#pragma once

#include "vulkan/Instance.h"
#include "vulkan/Device.h"
#include "palleon/graphics/GraphicDevice.h"

//#define _TRIANGLEDRAW_TEST

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
		           CVulkanGraphicDevice(const CVector2&, float);
		virtual    ~CVulkanGraphicDevice();
		
		void       Initialize();
		
		void       CreateInstance();
		void       CreateDebugReportCallback();
		void       CreateDevice(VkPhysicalDevice);
		
		VkRenderPass    CreateRenderPass(VkFormat);
		
		VkCommandPool                CreateCommandPool(uint32_t);
		std::vector<VkCommandBuffer> AllocateCommandBuffers(VkCommandPool, uint32_t);
		void                         FreeCommandBuffer(VkCommandPool, VkCommandBuffer);
		
		void       CreateSwapChain(VkSurfaceFormatKHR, VkExtent2D);
		void       PrepareSwapChainImages();
		void       CreateSwapChainImageViews(VkFormat);
		void       CreateSwapChainFramebuffers(VkRenderPass, VkExtent2D);
		
		std::vector<VkPhysicalDevice>   GetPhysicalDevices();
		std::vector<uint32_t>           GetRenderQueueFamilies(VkPhysicalDevice);
		std::vector<VkSurfaceFormatKHR> GetDeviceSurfaceFormats(VkPhysicalDevice);
		
		void                            BuildClearCommandList(VkCommandBuffer, VkImage, VkExtent2D, VkRenderPass, VkFramebuffer);
		
#ifdef _TRIANGLEDRAW_TEST
		void                            CreateTriangleDrawPipeline();
		void                            CreateTriangleVertexBuffer();
#endif
		
		Framework::Vulkan::CInstance     m_vkInstance;
		VkDebugReportCallbackEXT         m_debugReportCallback = VK_NULL_HANDLE;
		VkSurfaceKHR                     m_surface = VK_NULL_HANDLE;
		VkExtent2D                       m_surfaceExtents;
		Framework::Vulkan::CDevice       m_device;
		VkPhysicalDeviceMemoryProperties m_physicalDeviceMemoryProperties;
		VkQueue                          m_queue = VK_NULL_HANDLE;
		VkCommandPool                    m_commandPool = VK_NULL_HANDLE;
		VkRenderPass                     m_renderPass = VK_NULL_HANDLE;
		VkSemaphore                      m_imageAcquireSemaphore = VK_NULL_HANDLE;
		VkSemaphore                      m_renderCompleteSemaphore = VK_NULL_HANDLE;
		VkSwapchainKHR                   m_swapChain = VK_NULL_HANDLE;
		std::vector<VkImage>             m_swapChainImages;
		std::vector<VkImageView>         m_swapChainImageViews;
		
		std::vector<VkFramebuffer>       m_swapChainFramebuffers;
		
#ifdef _TRIANGLEDRAW_TEST
		VkPipeline                       m_triangleDrawPipeline;
		VertexBufferPtr                  m_triangleVertexBuffer;
#endif
	};
}
