#pragma once

#include "../vulkan/VulkanGraphicDevice.h"

namespace Palleon
{
	class CUnixVulkanGraphicDevice : public CVulkanGraphicDevice
	{
	public:
		static void    CreateInstance(xcb_connection_t*, xcb_window_t);
		static void    DestroyInstance();
		
	private:
		CUnixVulkanGraphicDevice(xcb_connection_t*, xcb_window_t);

		void CreateSurface();

		xcb_connection_t*    m_connection = nullptr;
		xcb_window_t         m_window = 0;
	};
}
