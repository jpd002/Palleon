#import <UIKit/UIKit.h>
#include "palleon/ios/IosResourceManager.h"
#include "palleon/GraphicDevice.h"
#include "make_unique.h"
#include "StdStream.h"

using namespace Palleon;

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

std::string CIosResourceManager::MakeResourcePath(const std::string& name) const
{
	NSString* bundlePath = [[NSBundle mainBundle] bundlePath];
	NSString* fullPath = [NSString stringWithFormat: @"%@/data/%s", bundlePath, name.c_str()];
	return std::string([fullPath UTF8String]);
}

CResourceManager::StreamPtr CIosResourceManager::MakeResourceStream(const std::string& name) const
{
	auto resourcePath = MakeResourcePath(name);
	return std::make_unique<Framework::CStdStream>(resourcePath.c_str(), "rb");
}

