#pragma once

#include "PalleonEngine.h"

class CWaterEffectProvider : public Palleon::CEffectProvider
{
public:
							CWaterEffectProvider();

	Palleon::EffectPtr		GetEffectForRenderable(Palleon::CMesh*, bool) override;

private:
	Palleon::EffectPtr		m_effect;
};
