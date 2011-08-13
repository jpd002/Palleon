#include <assert.h>
#include <algorithm>
#include "athena/GraphicDevice.h"
#include "athena/Vector3.h"
#include "athena/Vector2.h"

using namespace Athena;

CGraphicDevice* CGraphicDevice::m_instance = NULL;

CGraphicDevice::CGraphicDevice()
: m_screenSize(0, 0)
, m_drawCallCount(0)
, m_frameRate(0)
{

}

CGraphicDevice::~CGraphicDevice()
{

}

CGraphicDevice& CGraphicDevice::GetInstance()
{
	assert(m_instance != NULL);
	return (*m_instance);
}

CVector2 CGraphicDevice::GetScreenSize() const
{
	return m_screenSize;
}

uint32 CGraphicDevice::GetDrawCallCount() const
{
	return m_drawCallCount;
}

float CGraphicDevice::GetFrameRate() const
{
	return m_frameRate;
}

void CGraphicDevice::AddViewport(CViewport* viewport)
{
	assert(std::find(m_viewports.begin(), m_viewports.end(), viewport) == m_viewports.end());
	m_viewports.push_back(viewport);
}

void CGraphicDevice::RemoveViewport(CViewport* viewport)
{
	ViewportList::iterator viewportIterator = std::find(m_viewports.begin(), m_viewports.end(), viewport);
	assert(viewportIterator != m_viewports.end());
	m_viewports.erase(viewportIterator);
}
