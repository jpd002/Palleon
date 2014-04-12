#include "BspMapPass.h"

CBspMapPass::CBspMapPass()
: m_textureSource(TEXTURE_SOURCE_DIFFUSE)
, m_blendingFunction(Palleon::TEXTURE_COMBINE_MODULATE)
{
	m_uvMatrix = CMatrix4::MakeIdentity();
}

CBspMapPass::~CBspMapPass()
{

}

void CBspMapPass::Update(float dt)
{
	m_uvMatrix = CMatrix4::MakeIdentity();
	for(unsigned int i = 0; i < m_tcMods.size(); i++)
	{
		const BspMapTcModPtr& tcMod(m_tcMods[i]);
		tcMod->Update(dt);
		m_uvMatrix = m_uvMatrix * tcMod->GetMatrix();
	}
}

Palleon::TexturePtr CBspMapPass::GetTexture() const
{
	return m_texture;
}

void CBspMapPass::SetTexture(const Palleon::TexturePtr& texture)
{
	m_texture = texture;
}

CBspMapPass::TEXTURE_SOURCE CBspMapPass::GetTextureSource() const
{
	return m_textureSource;
}

void CBspMapPass::SetTextureSource(CBspMapPass::TEXTURE_SOURCE textureSource)
{
	m_textureSource = textureSource;
}

Palleon::TEXTURE_COMBINE_MODE CBspMapPass::GetBlendingFunction() const
{
	return m_blendingFunction;
}

void CBspMapPass::SetBlendingFunction(Palleon::TEXTURE_COMBINE_MODE blendingFunction)
{
	m_blendingFunction = blendingFunction;
}

const CMatrix4& CBspMapPass::GetUvMatrix() const
{
	return m_uvMatrix;
}

void CBspMapPass::AddTcMod(const BspMapTcModPtr& tcMod)
{
	m_tcMods.push_back(tcMod);
}
