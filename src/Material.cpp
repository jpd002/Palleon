#include <assert.h>
#include "athena/Material.h"

using namespace Athena;

CMaterial::CMaterial()
: m_isTransparent(false)
, m_color(1.0f, 1.0f, 1.0f, 1.0f)
, m_cullingMode(CULLING_CCW)
{
	for(unsigned int i = 0; i < MAX_TEXTURE_SLOTS; i++)
	{
		m_textureMatrices[i] = CMatrix4::MakeIdentity();
		m_textureCoordSources[i] = TEXTURE_COORD_UV0;
		m_textureCombineModes[i] = TEXTURE_COMBINE_MODULATE;
	}
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
	return m_textures[0].get() < rhs.m_textures[0].get();
}

bool CMaterial::GetIsTransparent() const
{
	return m_isTransparent;
}

void CMaterial::SetIsTransparent(bool isTransparent)
{
	m_isTransparent = isTransparent;
}

CULLING_MODE CMaterial::GetCullingMode() const
{
	return m_cullingMode;
}

void CMaterial::SetCullingMode(CULLING_MODE cullingMode)
{
	m_cullingMode = cullingMode;
}

TexturePtr CMaterial::GetTexture(unsigned int slot) const
{
	assert(slot < MAX_TEXTURE_SLOTS);
	if(slot >= MAX_TEXTURE_SLOTS) return TexturePtr();
	return m_textures[slot];
}

void CMaterial::SetTexture(unsigned int slot, const TexturePtr& texture)
{
	assert(slot < MAX_TEXTURE_SLOTS);
	if(slot >= MAX_TEXTURE_SLOTS) return;
	m_textures[slot] = texture;
}

const CMatrix4& CMaterial::GetTextureMatrix(unsigned int slot) const
{
	assert(slot < MAX_TEXTURE_SLOTS);
	if(slot >= MAX_TEXTURE_SLOTS) return m_textureMatrices[0];
	return m_textureMatrices[slot];
}

void CMaterial::SetTextureMatrix(unsigned int slot, const CMatrix4& matrix)
{
	assert(slot < MAX_TEXTURE_SLOTS);
	if(slot >= MAX_TEXTURE_SLOTS) return;
	m_textureMatrices[slot] = matrix;
}

TEXTURE_COORD_SOURCE CMaterial::GetTextureCoordSource(unsigned int slot) const
{
	assert(slot < MAX_TEXTURE_SLOTS);
	if(slot >= MAX_TEXTURE_SLOTS) return TEXTURE_COORD_UV0;
	return m_textureCoordSources[slot];
}

void CMaterial::SetTextureCoordSource(unsigned int slot, TEXTURE_COORD_SOURCE source)
{
	assert(slot < MAX_TEXTURE_SLOTS);
	if(slot >= MAX_TEXTURE_SLOTS) return;
	m_textureCoordSources[slot] = source;
}

TEXTURE_COMBINE_MODE CMaterial::GetTextureCombineMode(unsigned int slot) const
{
	assert(slot < MAX_TEXTURE_SLOTS);
	if(slot >= MAX_TEXTURE_SLOTS) return TEXTURE_COMBINE_MODULATE;
	return m_textureCombineModes[slot];
}

void CMaterial::SetTextureCombineMode(unsigned int slot, TEXTURE_COMBINE_MODE combineMode)
{
	assert(slot < MAX_TEXTURE_SLOTS);
	if(slot >= MAX_TEXTURE_SLOTS) return;
	m_textureCombineModes[slot] = combineMode;
}

CColor CMaterial::GetColor() const
{
	return m_color;
}

void CMaterial::SetColor(const CColor& color)
{
	m_color = color;
}
