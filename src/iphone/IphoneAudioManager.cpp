#include "athena/iphone/IphoneAudioManager.h"
#include "athena/iphone/IphoneAudioClip.h"
#include <assert.h>

using namespace Athena;

CIphoneAudioManager::CIphoneAudioManager()
{

}

CIphoneAudioManager::~CIphoneAudioManager()
{

}

void CIphoneAudioManager::CreateInstance()
{
	assert(m_instance == nullptr);
	if(m_instance != nullptr) return;
	m_instance = new CIphoneAudioManager();
}

void CIphoneAudioManager::DestroyInstance()
{
	assert(m_instance != nullptr);
	delete m_instance;
	m_instance = nullptr;
}

AudioClipPtr CIphoneAudioManager::CreateAudioClip(const char* path)
{
	return std::make_shared<CIphoneAudioClip>(path);
}
