#include <assert.h>
#include <algorithm>
#include "palleon/graphics/GraphicDevice.h"
#include "math/Vector3.h"
#include "math/Vector2.h"

using namespace Palleon;

CGraphicDevice* CGraphicDevice::m_instance = nullptr;

CGraphicDevice::CGraphicDevice()
{

}

CGraphicDevice::~CGraphicDevice()
{

}

CGraphicDevice& CGraphicDevice::GetInstance()
{
	assert(m_instance != nullptr);
	return (*m_instance);
}

CVector2 CGraphicDevice::GetScreenSize() const
{
	return m_screenSize;
}

CVector2 CGraphicDevice::GetScaledScreenSize() const
{
	return m_scaledScreenSize;
}

uint32 CGraphicDevice::GetDrawCallCount() const
{
	return m_drawCallCount;
}

float CGraphicDevice::GetFrameRate() const
{
	return m_frameRate;
}

EffectProviderPtr CGraphicDevice::GetDefaultEffectProvider() const
{
	return m_defaultEffectProvider;
}

void CGraphicDevice::AddViewport(CViewport* viewport)
{
	assert(viewport != nullptr);
	assert(std::find(std::begin(m_viewports), std::end(m_viewports), viewport) == std::end(m_viewports));
	m_viewports.push_back(viewport);
}

void CGraphicDevice::AddViewportAfter(CViewport* reference, CViewport* viewport)
{
	assert(std::find(std::begin(m_viewports), std::end(m_viewports), viewport) == std::end(m_viewports));

	auto viewportIterator(std::find(std::begin(m_viewports), std::end(m_viewports), reference));
	assert(viewportIterator != m_viewports.end());

	if(viewportIterator == m_viewports.end()) return;
	std::advance(viewportIterator, 1);
	m_viewports.insert(viewportIterator, viewport);
}

void CGraphicDevice::RemoveViewport(CViewport* viewport)
{
	auto viewportIterator = std::find(std::begin(m_viewports), std::end(m_viewports), viewport);
	assert(viewportIterator != std::end(m_viewports));
	m_viewports.erase(viewportIterator);
}
