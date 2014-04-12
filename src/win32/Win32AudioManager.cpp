#include "palleon/win32/Win32AudioManager.h"
#include "palleon/win32/Win32AudioClip.h"
#include <assert.h>

using namespace Palleon;

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
