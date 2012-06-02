#include "athena/ConfigManager.h"
#include "athena/ResourceManager.h"

using namespace Athena;

CConfigManager* CConfigManager::m_instance = NULL;

CConfigManager::CConfigManager()
: m_config(CResourceManager::GetInstance().MakeResourcePath("AppConfig.xml"))
{

}

CConfigManager::~CConfigManager()
{

}

Framework::CConfig& CConfigManager::GetConfig()
{
	return m_config;
}

CConfigManager& CConfigManager::GetInstance()
{
	return (*m_instance);
}

void CConfigManager::CreateInstance()
{
	m_instance = new CConfigManager();
}

void CConfigManager::DestroyInstance()
{
	delete m_instance;
}
