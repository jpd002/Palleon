#include "athena/ResourceManager.h"
#include "athena/GraphicDevice.h"
#include <zlib.h>
#include <assert.h>

using namespace Athena;

CResourceManager* CResourceManager::m_instance = NULL;

CResourceManager::CResourceManager()
{

}

CResourceManager::~CResourceManager()
{

}

CResourceManager& CResourceManager::GetInstance()
{
	assert(m_instance != NULL);
	return (*m_instance);
}

//Texture
//-------------------------------------------------------------------

TexturePtr CResourceManager::GetTexture(const char* fileName) const
{
	unsigned int resId = MakeCrc(fileName);
	auto textureIterator(m_textures.find(resId));
	assert(textureIterator != std::end(m_textures));
	if(textureIterator == std::end(m_textures))
	{
		return TexturePtr();
	}
	return textureIterator->second;
}

void CResourceManager::LoadTexture(const char* name, const char* localPath)
{
	if(!localPath) localPath = name;
	unsigned int resId = MakeCrc(name);
	std::string path = MakeResourcePath(localPath);

	assert(m_textures.find(resId) == std::end(m_textures));
	TexturePtr result = CGraphicDevice::GetInstance().CreateTextureFromFile(path.c_str());
	m_textures[resId] = result;
}

void CResourceManager::ReleaseTexture(const char* fileName)
{
	unsigned int resId = MakeCrc(fileName);
	auto textureIterator(m_textures.find(resId));
	assert(textureIterator != std::end(m_textures));
	m_textures.erase(textureIterator);
}

//Font Descriptor
//-------------------------------------------------------------------

const CFontDescriptor* CResourceManager::GetFontDescriptor(const char* fileName) const
{
	unsigned int resId = MakeCrc(fileName);
	auto fontDescriptorIterator(m_fontDescriptors.find(resId));
	assert(fontDescriptorIterator != std::end(m_fontDescriptors));
	if(fontDescriptorIterator == std::end(m_fontDescriptors))
	{
		return NULL;
	}
	return fontDescriptorIterator->second;
}

void CResourceManager::LoadFontDescriptor(const char* name, const char* localPath)
{
	if(!localPath) localPath = name;
	unsigned int resId = MakeCrc(name);
	std::string path = MakeResourcePath(localPath);

	assert(m_fontDescriptors.find(resId) == std::end(m_fontDescriptors));
	CFontDescriptor* descriptor = new CFontDescriptor();
	descriptor->Load(path.c_str());
	m_fontDescriptors[resId] = descriptor;
}

void CResourceManager::ReleaseFontDescriptor(const char* fileName)
{
	unsigned int resId = MakeCrc(fileName);
	auto fontDescriptorIterator(m_fontDescriptors.find(resId));
	assert(fontDescriptorIterator != std::end(m_fontDescriptors));
	m_fontDescriptors.erase(fontDescriptorIterator);
}

//NinePatch Descriptor
//-------------------------------------------------------------------

const CNinePatchDescriptor* CResourceManager::GetNinePatchDescriptor(const char* fileName) const
{
	unsigned int resId = MakeCrc(fileName);
	auto ninePatchDescriptorIterator(m_ninePatchDescriptors.find(resId));
	assert(ninePatchDescriptorIterator != std::end(m_ninePatchDescriptors));
	if(ninePatchDescriptorIterator == std::end(m_ninePatchDescriptors))
	{
		return NULL;
	}
	return ninePatchDescriptorIterator->second;
}

void CResourceManager::LoadNinePatchDescriptor(const char* name, const char* localPath)
{
	if(!localPath) localPath = name;
	unsigned int resId = MakeCrc(name);
	std::string path = MakeResourcePath(localPath);

	assert(m_ninePatchDescriptors.find(resId) == std::end(m_ninePatchDescriptors));
	CNinePatchDescriptor* descriptor = new CNinePatchDescriptor();
	descriptor->Load(path.c_str());
	m_ninePatchDescriptors[resId] = descriptor;
}

void CResourceManager::ReleaseNinePatchDescriptor(const char* fileName)
{
	unsigned int resId = MakeCrc(fileName);
	auto ninePatchDescriptorIterator(m_ninePatchDescriptors.find(resId));
	assert(ninePatchDescriptorIterator != std::end(m_ninePatchDescriptors));
	m_ninePatchDescriptors.erase(ninePatchDescriptorIterator);
}

uint32 CResourceManager::MakeCrc(const char* inputString)
{
	uLong crc = 0;
	return crc32(crc, reinterpret_cast<const Bytef*>(inputString), strlen(inputString));
}
