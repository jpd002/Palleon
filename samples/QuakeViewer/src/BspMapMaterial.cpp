#include "BspMapMaterial.h"

CBspMapMaterial::CBspMapMaterial()
: m_isSky(false)
{
	m_passes.reserve(MAX_PASS);
}

CBspMapMaterial::~CBspMapMaterial()
{

}

void CBspMapMaterial::Update(float dt)
{
	for(unsigned int i = 0; i < m_passes.size(); i++)
	{
		const BspMapPassPtr& pass(m_passes[i]);
		pass->Update(dt);
	}
}

void CBspMapMaterial::AddPass(const BspMapPassPtr& pass)
{
	m_passes.push_back(pass);
}

BspMapPassPtr CBspMapMaterial::GetPass(unsigned int passIdx) const
{
	return m_passes[passIdx];
}

unsigned int CBspMapMaterial::GetPassCount() const
{
	return m_passes.size();
}

bool CBspMapMaterial::GetIsSky() const
{
	return m_isSky;
}

void CBspMapMaterial::SetIsSky(bool isSky)
{
	m_isSky = isSky;
}
