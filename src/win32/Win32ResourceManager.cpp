#include <assert.h>
#include <d3dx9.h>
#include "athena/win32/Win32ResourceManager.h"
#include "athena/GraphicDevice.h"

#define DATA_BASE	"./data/"

using namespace Athena;

CWin32ResourceManager::CWin32ResourceManager(IDirect3DDevice9* device)
: m_device(device)
{

}

CWin32ResourceManager::~CWin32ResourceManager()
{
	ReleaseAllResources();
}

void CWin32ResourceManager::CreateInstance(IDirect3DDevice9* device)
{
	assert(m_instance == NULL);
	if(m_instance != NULL) return;
	m_instance = new CWin32ResourceManager(device);
}

void CWin32ResourceManager::DestroyInstance()
{
	assert(m_instance != NULL);
	delete m_instance;
	m_instance = NULL;
}

void CWin32ResourceManager::ReleaseAllResources()
{

}

std::string CWin32ResourceManager::MakeResourcePath(const char* name) const
{
	return std::string(DATA_BASE) + name;
}

void CWin32ResourceManager::LoadTexture(const char* name)
{
	unsigned int resId = MakeCrc(name);
	std::string path = MakeResourcePath(name);
	assert(m_textures.find(resId) == m_textures.end());
	TexturePtr result = CGraphicDevice::GetInstance().CreateTextureFromFile(path.c_str());
	m_textures[resId] = result;
}

void CWin32ResourceManager::LoadFontDescriptor(const char* name)
{
	unsigned int resId = MakeCrc(name);
	std::string path = MakeResourcePath(name);

	CFontDescriptor* descriptor = new CFontDescriptor();
	descriptor->Load(path.c_str());
	m_fontDescriptors[resId] = descriptor;
}
