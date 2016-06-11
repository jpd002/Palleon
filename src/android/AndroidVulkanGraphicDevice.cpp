#include "palleon/android/AndroidVulkanGraphicDevice.h"
#include "palleon/Log.h"
#include "vulkan/StructDefs.h"

using namespace Palleon;

CAndroidVulkanGraphicDevice::CAndroidVulkanGraphicDevice(ANativeWindow* nativeWindow, int width, int height, float density)
: CVulkanGraphicDevice(CVector2(width, height), density)
, m_window(nativeWindow)
{
	CreateSurface();
	Initialize();
}

CAndroidVulkanGraphicDevice::~CAndroidVulkanGraphicDevice()
{

}

void CAndroidVulkanGraphicDevice::CreateInstance(ANativeWindow* nativeWindow, int width, int height, float density)
{
	assert(m_instance == nullptr);
	if(m_instance != nullptr) return;
	m_instance = new CAndroidVulkanGraphicDevice(nativeWindow, width, height, density);
}

void CAndroidVulkanGraphicDevice::CreateSurface()
{
	assert(!m_vkInstance.IsEmpty());
	
	auto surfaceCreateInfo = Framework::Vulkan::AndroidSurfaceCreateInfoKHR();
	surfaceCreateInfo.window = m_window;
	
	auto result = m_vkInstance.vkCreateAndroidSurfaceKHR(m_vkInstance, &surfaceCreateInfo, nullptr, &m_surface);
	CHECKVULKANERROR(result);
	
	CLog::GetInstance().Print("Created surface.");
}

void CAndroidVulkanGraphicDevice::PresentBackBuffer()
{
	
}
