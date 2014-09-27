#pragma once

#include "PalleonEngine.h"
#include "palleon/win32/Dx11Effect.h"

class CDx11WaterEffectProvider : public Palleon::CEffectProvider
{
public:
							CDx11WaterEffectProvider(ID3D11Device*, ID3D11DeviceContext*);

	Palleon::EffectPtr		GetEffectForRenderable(Palleon::CMesh*, bool) override;

private:
	Palleon::EffectPtr		m_effect;
};
