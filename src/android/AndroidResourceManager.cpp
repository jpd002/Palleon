#include "palleon/android/AndroidResourceManager.h"

using namespace Palleon;

CAndroidResourceManager::CAndroidResourceManager()
{

}

CAndroidResourceManager::~CAndroidResourceManager()
{

}

void CAndroidResourceManager::CreateInstance()
{
	assert(m_instance == NULL);
	if(m_instance != NULL) return;
	m_instance = new CAndroidResourceManager();
}

void CAndroidResourceManager::DestroyInstance()
{
	assert(m_instance != NULL);
	if(m_instance == NULL) return;
	delete m_instance;
}

std::string CAndroidResourceManager::MakeResourcePath(const std::string& name) const
{
	return name;
}
