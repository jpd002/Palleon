#include <assert.h>
#include "athena/Material.h"

using namespace Athena;

CMaterial::CMaterial()
: m_isTransparent(false)
, m_color(1.0f, 1.0f, 1.0f, 1.0f)
, m_cullingMode(CULLING_CCW)
{
	for(auto slotIterator = std::begin(m_textureSlots); 
		slotIterator != std::end(m_textureSlots); slotIterator++)
	{
		TEXTURE_SLOT& textureSlot(*slotIterator);
		textureSlot.matrix = CMatrix4::MakeIdentity();
		textureSlot.coordSource = TEXTURE_COORD_UV0;
		textureSlot.combineMode = TEXTURE_COMBINE_MODULATE;
		textureSlot.addressModeU = TEXTURE_ADDRESS_CLAMP;
		textureSlot.addressModeV = TEXTURE_ADDRESS_CLAMP;
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
	return m_textureSlots[0].texture.get() < rhs.m_textureSlots[0].texture.get();
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

CColor CMaterial::GetColor() const
{
	return m_color;
}

void CMaterial::SetColor(const CColor& color)
{
	m_color = color;
}

TexturePtr CMaterial::GetTexture(unsigned int slot) const
{
	assert(slot < MAX_TEXTURE_SLOTS);
	if(slot >= MAX_TEXTURE_SLOTS) return TexturePtr();
	return m_textureSlots[slot].texture;
}

void CMaterial::SetTexture(unsigned int slot, const TexturePtr& texture)
{
	assert(slot < MAX_TEXTURE_SLOTS);
	if(slot >= MAX_TEXTURE_SLOTS) return;
	m_textureSlots[slot].texture = texture;
}

const CMatrix4& CMaterial::GetTextureMatrix(unsigned int slot) const
{
	assert(slot < MAX_TEXTURE_SLOTS);
	if(slot >= MAX_TEXTURE_SLOTS) return m_textureSlots[0].matrix;
	return m_textureSlots[slot].matrix;
}

void CMaterial::SetTextureMatrix(unsigned int slot, const CMatrix4& matrix)
{
	assert(slot < MAX_TEXTURE_SLOTS);
	if(slot >= MAX_TEXTURE_SLOTS) return;
	m_textureSlots[slot].matrix = matrix;
}

TEXTURE_COORD_SOURCE CMaterial::GetTextureCoordSource(unsigned int slot) const
{
	assert(slot < MAX_TEXTURE_SLOTS);
	if(slot >= MAX_TEXTURE_SLOTS) return TEXTURE_COORD_UV0;
	return m_textureSlots[slot].coordSource;
}

void CMaterial::SetTextureCoordSource(unsigned int slot, TEXTURE_COORD_SOURCE source)
{
	assert(slot < MAX_TEXTURE_SLOTS);
	if(slot >= MAX_TEXTURE_SLOTS) return;
	m_textureSlots[slot].coordSource = source;
}

TEXTURE_COMBINE_MODE CMaterial::GetTextureCombineMode(unsigned int slot) const
{
	assert(slot < MAX_TEXTURE_SLOTS);
	if(slot >= MAX_TEXTURE_SLOTS) return TEXTURE_COMBINE_MODULATE;
	return m_textureSlots[slot].combineMode;
}

void CMaterial::SetTextureCombineMode(unsigned int slot, TEXTURE_COMBINE_MODE combineMode)
{
	assert(slot < MAX_TEXTURE_SLOTS);
	if(slot >= MAX_TEXTURE_SLOTS) return;
	m_textureSlots[slot].combineMode = combineMode;
}

TEXTURE_ADDRESS_MODE CMaterial::GetTextureAddressModeU(unsigned int slot) const
{
	assert(slot < MAX_TEXTURE_SLOTS);
	if(slot >= MAX_TEXTURE_SLOTS) return TEXTURE_ADDRESS_CLAMP;
	return m_textureSlots[slot].addressModeU;
}

void CMaterial::SetTextureAddressModeU(unsigned int slot, TEXTURE_ADDRESS_MODE addressMode)
{
	assert(slot < MAX_TEXTURE_SLOTS);
	if(slot >= MAX_TEXTURE_SLOTS) return;
	m_textureSlots[slot].addressModeU = addressMode;
}

TEXTURE_ADDRESS_MODE CMaterial::GetTextureAddressModeV(unsigned int slot) const
{
	assert(slot < MAX_TEXTURE_SLOTS);
	if(slot >= MAX_TEXTURE_SLOTS) return TEXTURE_ADDRESS_CLAMP;
	return m_textureSlots[slot].addressModeV;
}

void CMaterial::SetTextureAddressModeV(unsigned int slot, TEXTURE_ADDRESS_MODE addressMode)
{
	assert(slot < MAX_TEXTURE_SLOTS);
	if(slot >= MAX_TEXTURE_SLOTS) return;
	m_textureSlots[slot].addressModeV = addressMode;
}
