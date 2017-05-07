#include "palleon/win32/Win32VulkanGraphicDevice.h"
#include "palleon/Log.h"
#include "vulkan/StructDefs.h"

using namespace Palleon;

CWin32VulkanGraphicDevice::CWin32VulkanGraphicDevice(HINSTANCE instanceHandle, HWND windowHandle)
: CVulkanGraphicDevice(CVector2(0.0f, 0.0f), 1.0f)
{
	CreateSurface(instanceHandle, windowHandle);
	Initialize();
}

void CWin32VulkanGraphicDevice::CreateInstance(HINSTANCE instanceHandle, HWND windowHandle)
{
	assert(m_instance == nullptr);
	if(m_instance != nullptr) return;
	m_instance = new CWin32VulkanGraphicDevice(instanceHandle, windowHandle);
}

void CWin32VulkanGraphicDevice::DestroyInstance()
{
	assert(m_instance != nullptr);
	delete m_instance;
	m_instance = nullptr;
}

void CWin32VulkanGraphicDevice::CreateSurface(HINSTANCE instanceHandle, HWND windowHandle)
{
	assert(!m_vkInstance.IsEmpty());
	
	auto surfaceCreateInfo = Framework::Vulkan::Win32SurfaceCreateInfoKHR();
	surfaceCreateInfo.hinstance = instanceHandle;
	surfaceCreateInfo.hwnd      = windowHandle;

	auto result = m_vkInstance.vkCreateWin32SurfaceKHR(m_vkInstance, &surfaceCreateInfo, nullptr, &m_surface);
	CHECKVULKANERROR(result);

	CLog::GetInstance().Print("Created surface.");
}
