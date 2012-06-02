#include "IphoneResourceManager.h"
#include "athena/GraphicDevice.h"

using namespace Athena;

CIphoneResourceManager::CIphoneResourceManager()
{

}

CIphoneResourceManager::~CIphoneResourceManager()
{

}

void CIphoneResourceManager::CreateInstance()
{
	assert(m_instance == NULL);
	if(m_instance != NULL) return;
	m_instance = new CIphoneResourceManager();
}

void CIphoneResourceManager::DestroyInstance()
{
	assert(m_instance != NULL);
	if(m_instance == NULL) return;
	delete m_instance;
}

std::string CIphoneResourceManager::MakeResourcePath(const char* name) const
{
	NSString* bundlePath = [[NSBundle mainBundle] bundlePath];
	NSString* fullPath = [NSString stringWithFormat: @"%@/data/%s", bundlePath, name];
	return std::string([fullPath UTF8String]);
}
