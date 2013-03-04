#include "athena/ios/IosAudioManager.h"
#include "athena/ios/IosAudioClip.h"
#include <assert.h>

using namespace Athena;

CIosAudioManager::CIosAudioManager()
{

}

CIosAudioManager::~CIosAudioManager()
{

}

void CIosAudioManager::CreateInstance()
{
	assert(m_instance == nullptr);
	if(m_instance != nullptr) return;
	m_instance = new CIosAudioManager();
}

void CIosAudioManager::DestroyInstance()
{
	assert(m_instance != nullptr);
	delete m_instance;
	m_instance = nullptr;
}

AudioClipPtr CIosAudioManager::CreateAudioClip(const char* path)
{
	return std::make_shared<CIosAudioClip>(path);
}
