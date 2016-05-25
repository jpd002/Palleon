#pragma once

#include "vulkan/Instance.h"
#include "vulkan/Device.h"
#include "palleon/graphics/GraphicDevice.h"

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
		
	protected:
		           CVulkanGraphicDevice(const CVector2&, float);
		virtual    ~CVulkanGraphicDevice();
		
		void       Initialize();
		
		void       CreateInstance();
		void       CreateDevice(VkPhysicalDevice);
		
		std::vector<VkPhysicalDevice>    GetPhysicalDevices();
		
		Framework::Vulkan::CInstance    m_vkInstance;
		Framework::Vulkan::CDevice      m_device;
	};
}
