#include "IosResourceManager.h"
#include "athena/GraphicDevice.h"

using namespace Athena;

CIosResourceManager::CIosResourceManager()
{

}

CIosResourceManager::~CIosResourceManager()
{

}

void CIosResourceManager::CreateInstance()
{
	assert(m_instance == NULL);
	if(m_instance != NULL) return;
	m_instance = new CIosResourceManager();
}

void CIosResourceManager::DestroyInstance()
{
	assert(m_instance != NULL);
	if(m_instance == NULL) return;
	delete m_instance;
}

std::string CIosResourceManager::MakeResourcePath(const char* name) const
{
	NSString* bundlePath = [[NSBundle mainBundle] bundlePath];
	NSString* fullPath = [NSString stringWithFormat: @"%@/data/%s", bundlePath, name];
	return std::string([fullPath UTF8String]);
}
