#include "palleon/win32/Dx11ContextManager.h"

using namespace Palleon;

CDx11ContextManager::CDx11ContextManager()
{

}

CDx11ContextManager::~CDx11ContextManager()
{

}

ID3D11DeviceContext* CDx11ContextManager::GetCurrentDeviceContext() const
{
	auto currentThreadId = GetCurrentThreadId();
	auto deviceContextIterator = m_deviceContexts.find(currentThreadId);
	assert(deviceContextIterator != std::end(m_deviceContexts));
	if(deviceContextIterator == std::end(m_deviceContexts))
	{
		return nullptr;
	}
	return deviceContextIterator->second;
}

void CDx11ContextManager::SetCurrentDeviceContext(const DeviceContextPtr& deviceContext)
{
	auto currentThreadId = GetCurrentThreadId();
	auto deviceContextIterator = m_deviceContexts.find(currentThreadId);
	if(deviceContext.IsEmpty())
	{
		m_deviceContexts.erase(deviceContextIterator);
		return;
	}
	assert(deviceContextIterator == std::end(m_deviceContexts));
	m_deviceContexts[currentThreadId] = deviceContext;
}
