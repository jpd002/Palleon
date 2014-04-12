#include "palleon/resources/TextureResource.h"
#include "palleon/TextureLoader.h"

using namespace Palleon;

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
