#pragma once

#include <android/native_window.h>
#include "../vulkan/VulkanGraphicDevice.h"

namespace Palleon
{
	class CAndroidVulkanGraphicDevice : public CVulkanGraphicDevice
	{
	public:
		static void						CreateInstance(ANativeWindow*, int, int, float);
		
		void							PresentBackBuffer();
		
		virtual SharedGraphicContextPtr	CreateSharedContext() override;
		
	private:
										CAndroidVulkanGraphicDevice(ANativeWindow*, int, int, float);
		virtual							~CAndroidVulkanGraphicDevice();
		
		void							CreateSurface();
		
		ANativeWindow*					m_window = nullptr;
	};
}
