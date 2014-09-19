#include <iostream>
#include "palleon/android/AndroidGraphicDevice.h"
#include "palleon/gles/GlEsVertexBuffer.h"

using namespace Palleon;

CAndroidGraphicDevice::CAndroidGraphicDevice(int width, int height)
: CGlEsGraphicDevice(CVector2(width, height), 1)
{
	Initialize();
}

CAndroidGraphicDevice::~CAndroidGraphicDevice()
{

}

void CAndroidGraphicDevice::CreateInstance(int width, int height)
{
	assert(m_instance == nullptr);
	if(m_instance != nullptr) return;
	m_instance = new CAndroidGraphicDevice(width, height);
}
