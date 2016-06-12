#include "palleon/unix/UnixVulkanGraphicDevice.h"
#include "palleon/Log.h"
#include "vulkan/StructDefs.h"

using namespace Palleon;

CUnixVulkanGraphicDevice::CUnixVulkanGraphicDevice(xcb_connection_t* connection, xcb_window_t 
window)
: CVulkanGraphicDevice(CVector2(0.0f, 0.0f), 1.0f)
, m_connection(connection)
, m_window(window)
{
	CreateSurface();
	Initialize();
}

void CUnixVulkanGraphicDevice::CreateInstance(xcb_connection_t* connection, xcb_window_t window)
{
	assert(m_instance == nullptr);
	if(m_instance != nullptr) return;
	m_instance = new CUnixVulkanGraphicDevice(connection, window);
}

void CUnixVulkanGraphicDevice::DestroyInstance()
{
	assert(m_instance != nullptr);
	delete m_instance;
	m_instance = nullptr;
}

void CUnixVulkanGraphicDevice::CreateSurface()
{
	assert(!m_vkInstance.IsEmpty());
	
	auto surfaceCreateInfo = Framework::Vulkan::XcbSurfaceCreateInfoKHR();
	surfaceCreateInfo.connection = m_connection;
	surfaceCreateInfo.window     = m_window;

	auto result = m_vkInstance.vkCreateXcbSurfaceKHR(m_vkInstance, &surfaceCreateInfo, nullptr, &m_surface);
	CHECKVULKANERROR(result);

	CLog::GetInstance().Print("Created surface.");
}

