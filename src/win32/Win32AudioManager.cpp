#include "athena/win32/Win32AudioManager.h"
#include "athena/win32/Win32AudioClip.h"
#include <assert.h>

using namespace Athena;

CWin32AudioManager::CWin32AudioManager()
{

}

CWin32AudioManager::~CWin32AudioManager()
{

}

void CWin32AudioManager::CreateInstance()
{
	assert(m_instance == nullptr);
	if(m_instance != nullptr) return;
	m_instance = new CWin32AudioManager();
}

void CWin32AudioManager::DestroyInstance()
{
	assert(m_instance != nullptr);
	delete m_instance;
	m_instance = nullptr;
}

AudioClipPtr CWin32AudioManager::CreateAudioClip(const char*)
{
	return std::make_shared<CWin32AudioClip>();
}
