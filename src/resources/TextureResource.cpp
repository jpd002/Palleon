#include "athena/resources/TextureResource.h"
#include "athena/TextureLoader.h"

using namespace Athena;

CTextureResource::CTextureResource()
{

}

CTextureResource::~CTextureResource()
{

}

void CTextureResource::Load(const char* path)
{
	m_texture = CTextureLoader::CreateTextureFromFile(path);
}

TexturePtr CTextureResource::GetTexture() const
{
	return m_texture;
}
