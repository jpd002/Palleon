#include "athena/resources/TextureResource.h"
#include "athena/GraphicDevice.h"

using namespace Athena;

CTextureResource::CTextureResource()
{

}

CTextureResource::~CTextureResource()
{

}

void CTextureResource::Load(const char* path)
{
	m_texture = CGraphicDevice::GetInstance().CreateTextureFromFile(path);
}

TexturePtr CTextureResource::GetTexture() const
{
	return m_texture;
}
