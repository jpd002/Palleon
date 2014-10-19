#include "WaterEffectProvider.h"
#include "WaterEffect.h"

CWaterEffectProvider::CWaterEffectProvider()
{
	m_effect = std::make_shared<CWaterEffect>();
}

Palleon::EffectPtr CWaterEffectProvider::GetEffectForRenderable(Palleon::CMesh*, bool)
{
	return m_effect;
}
