#include "palleon/android/AndroidVulkanGraphicDevice.h"

using namespace Palleon;

CAndroidVulkanGraphicDevice::CAndroidVulkanGraphicDevice(ANativeWindow* nativeWindow, int width, int height, float density)
: CVulkanGraphicDevice(CVector2(width, height), density)
, m_window(nativeWindow)
{
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

SharedGraphicContextPtr CAndroidVulkanGraphicDevice::CreateSharedContext()
{
	return SharedGraphicContextPtr();
}

void CAndroidVulkanGraphicDevice::PresentBackBuffer()
{
	
}
