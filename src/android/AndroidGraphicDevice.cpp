#include <iostream>
#include "palleon/android/AndroidGraphicDevice.h"
#include "palleon/gles/GlEsVertexBuffer.h"

using namespace Palleon;

CAndroidGraphicDevice::CAndroidGraphicDevice(int width, int height, float density)
: CGlEsGraphicDevice(CVector2(width, height), density)
{
	Initialize();
}

CAndroidGraphicDevice::~CAndroidGraphicDevice()
{

}

void CAndroidGraphicDevice::CreateInstance(int width, int height, float density)
{
	assert(m_instance == nullptr);
	if(m_instance != nullptr) return;
	m_instance = new CAndroidGraphicDevice(width, height, density);
}

SharedGraphicContextPtr CAndroidGraphicDevice::CreateSharedContext()
{
	return SharedGraphicContextPtr();
}
