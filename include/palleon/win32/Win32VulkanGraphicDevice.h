#pragma once

#include "../vulkan/VulkanGraphicDevice.h"

namespace Palleon
{
	class CWin32VulkanGraphicDevice : public CVulkanGraphicDevice
	{
	public:
		static void    CreateInstance(HINSTANCE, HWND);
		static void    DestroyInstance();
		
	private:
		CWin32VulkanGraphicDevice(HINSTANCE, HWND);

		void CreateSurface(HINSTANCE, HWND);
	};
}
