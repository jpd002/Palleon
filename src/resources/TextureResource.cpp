#include "palleon/resources/TextureResource.h"
#include "palleon/graphics/TextureLoader.h"

using namespace Palleon;

CTextureResource::CTextureResource()
{

}

CTextureResource::~CTextureResource()
{

}

void CTextureResource::Load(Framework::CStream& inputStream)
{
	m_texture = CTextureLoader::CreateTextureFromStream(inputStream);
}

TexturePtr CTextureResource::GetTexture() const
{
	return m_texture;
}
