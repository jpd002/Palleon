#include <assert.h>
#include "athena/Material.h"

using namespace Athena;

CMaterial::CMaterial()
: m_renderType(RENDER_DIFFUSE)
, m_isTransparent(false)
{

}

CMaterial::~CMaterial()
{

}

MaterialPtr CMaterial::Create()
{
	return MaterialPtr(new CMaterial());
}

bool CMaterial::operator <(const CMaterial& rhs) const
{
	if(m_renderType == rhs.m_renderType)
	{
		return m_textures[0].get() < rhs.m_textures[0].get();
	}
	else
	{
		return m_renderType < rhs.m_renderType;
	}
}

bool CMaterial::GetIsTransparent() const
{
	return m_isTransparent;
}

void CMaterial::SetIsTransparent(bool isTransparent)
{
	m_isTransparent = isTransparent;
}

RENDER_TYPE CMaterial::GetRenderType() const
{
	return m_renderType;
}

void CMaterial::SetRenderType(RENDER_TYPE renderType)
{
	m_renderType = renderType;
}

TexturePtr CMaterial::GetTexture(unsigned int slot) const
{
	assert(slot < MAX_TEXTURE_SLOTS);
	return m_textures[slot];
}

void CMaterial::SetTexture(unsigned int slot, const TexturePtr& texture)
{
	assert(slot < MAX_TEXTURE_SLOTS);
	m_textures[slot] = texture;
}
