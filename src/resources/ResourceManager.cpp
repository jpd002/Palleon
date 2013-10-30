#include "athena/resources/ResourceManager.h"
#include <zlib.h>
#include <assert.h>

using namespace Athena;

CResourceManager* CResourceManager::m_instance = nullptr;

CResourceManager::CResourceManager()
{

}

CResourceManager::~CResourceManager()
{

}

CResourceManager& CResourceManager::GetInstance()
{
	assert(m_instance != nullptr);
	return *m_instance;
}

ResourcePtr CResourceManager::GetResource(const char* fileName) const
{
	unsigned int resId = MakeCrc(fileName);
	auto resourceIterator(m_resources.find(resId));
	assert(resourceIterator != std::end(m_resources));
	if(resourceIterator == std::end(m_resources))
	{
		return ResourcePtr();
	}
	return resourceIterator->second;
}

void CResourceManager::ReleaseResource(const char* fileName)
{
	unsigned int resId = MakeCrc(fileName);
	auto resourceIterator(m_resources.find(resId));
	assert(resourceIterator != std::end(m_resources));
	m_resources.erase(resourceIterator);
}

TexturePtr CResourceManager::GetTexture(const char* fileName) const
{
	auto texture = GetResource<CTextureResource>(fileName);
	if(!texture) return TexturePtr();
	return texture->GetTexture();
}

uint32 CResourceManager::MakeCrc(const char* inputString)
{
	uLong crc = 0;
	return crc32(crc, reinterpret_cast<const Bytef*>(inputString), strlen(inputString));
}
