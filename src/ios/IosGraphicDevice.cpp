#include <UIKit/UIKit.h>
#include "athena/ios/IosGraphicDevice.h"

using namespace Athena;

CIosGraphicDevice::CIosGraphicDevice(bool hasRetinaDisplay, const CVector2& screenSize)
: CGlEsGraphicDevice(screenSize, hasRetinaDisplay ? 2.f : 1.f)
{
	Initialize();
}

CIosGraphicDevice::~CIosGraphicDevice()
{

}

void CIosGraphicDevice::CreateInstance(bool hasRetinaDisplay, const CVector2& screenSize)
{
	assert(m_instance == NULL);
	if(m_instance != NULL) return;
	m_instance = new CIosGraphicDevice(hasRetinaDisplay, screenSize);
}

void CIosGraphicDevice::DestroyInstance()
{
	assert(m_instance != NULL);
	if(m_instance == NULL) return;
	delete m_instance;
}
