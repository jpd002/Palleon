#include "palleon/win32/Win32EmbedManager.h"

using namespace Palleon;

CWin32EmbedManager::CWin32EmbedManager()
{

}

CWin32EmbedManager::~CWin32EmbedManager()
{

}

void CWin32EmbedManager::CreateInstance()
{
	assert(m_instance == nullptr);
	if(m_instance != nullptr) return;
	m_instance = new CWin32EmbedManager();
}

void CWin32EmbedManager::DestroyInstance()
{
	assert(m_instance != nullptr);
	delete m_instance;
	m_instance = nullptr;
}

void CWin32EmbedManager::NotifyClient(const CEmbedRemoteCall& rpc)
{
	m_pendingEvents.push_back(rpc);
}

const CWin32EmbedManager::PendingEventArray& CWin32EmbedManager::GetPendingEvents() const
{
	return m_pendingEvents;
}

void CWin32EmbedManager::ClearPendingEvents()
{
	m_pendingEvents.clear();
}
