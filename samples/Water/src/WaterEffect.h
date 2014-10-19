#pragma once

#include "PalleonEngine.h"
#include "PlatformEffect.h"

class CWaterEffect : public Palleon::CPlatformEffect
{
public:
							CWaterEffect();

	void					UpdateConstantsInner(const Palleon::VIEWPORT_PARAMS&, Palleon::CMaterial*, const CMatrix4&) override;

private:
	static CShaderBuilder	CreateVertexShader();
	static CShaderBuilder	CreatePixelShader();
};
