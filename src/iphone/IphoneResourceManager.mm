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

void CIphoneResourceManager::ReleaseAllResources()
{
    
}

void CIphoneResourceManager::LoadTexture(const char* fileName)
{
    unsigned int resId = MakeCrc(fileName);
    std::string path = MakeResourcePath(fileName);

    TexturePtr result = CGraphicDevice::GetInstance().CreateTextureFromFile(path.c_str());
	assert(m_textures.find(resId) == m_textures.end());
    m_textures[resId] = result;
}

void CIphoneResourceManager::LoadFontDescriptor(const char* fileName)
{
    unsigned int resId = MakeCrc(fileName);
    std::string path = MakeResourcePath(fileName);

	CFontDescriptor* result = new CFontDescriptor();
	result->Load(path.c_str());
	assert(m_fontDescriptors.find(resId) == m_fontDescriptors.end());
	m_fontDescriptors[resId] = result;
}
