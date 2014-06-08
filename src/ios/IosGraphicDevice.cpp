#include <UIKit/UIKit.h>
#include "palleon/ios/IosGraphicDevice.h"

using namespace Palleon;

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
	assert(m_instance == nullptr);
	if(m_instance != nullptr) return;
	m_instance = new CIosGraphicDevice(hasRetinaDisplay, screenSize);
}

void CIosGraphicDevice::DestroyInstance()
{
	assert(m_instance != nullptr);
	if(m_instance == nullptr) return;
	delete m_instance;
}
