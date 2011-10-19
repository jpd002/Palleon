#include "athena/ResourceManager.h"
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

TexturePtr CResourceManager::GetTexture(const char* fileName) const
{
	unsigned int resId = MakeCrc(fileName);
	TextureMap::const_iterator textureIterator(m_textures.find(resId));
	assert(textureIterator != m_textures.end());
	if(textureIterator == m_textures.end())
	{
		return TexturePtr();
	}
	return textureIterator->second;
}

void CResourceManager::ReleaseTexture(const char* fileName)
{
	unsigned int resId = MakeCrc(fileName);
	TextureMap::const_iterator textureIterator(m_textures.find(resId));
	assert(textureIterator != m_textures.end());
	m_textures.erase(textureIterator);
}

const CFontDescriptor* CResourceManager::GetFontDescriptor(const char* fileName) const
{
	unsigned int resId = MakeCrc(fileName);
	FontDescriptorMap::const_iterator fontDescriptorIterator(m_fontDescriptors.find(resId));
	assert(fontDescriptorIterator != m_fontDescriptors.end());
	if(fontDescriptorIterator == m_fontDescriptors.end())
	{
		return NULL;
	}
	return fontDescriptorIterator->second;	
}

void CResourceManager::ReleaseFontDescriptor(const char* fileName)
{
	unsigned int resId = MakeCrc(fileName);
	FontDescriptorMap::const_iterator fontDescriptorIterator(m_fontDescriptors.find(resId));
	assert(fontDescriptorIterator != m_fontDescriptors.end());
	m_fontDescriptors.erase(fontDescriptorIterator);
}

uint32 CResourceManager::MakeCrc(const char* inputString)
{
	uLong crc = 0;
	return crc32(crc, reinterpret_cast<const Bytef*>(inputString), strlen(inputString));
}
