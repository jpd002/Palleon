#pragma once

#include "PalleonEngine.h"

class CWaterEffect : public Palleon::CPlatformGenericEffect
{
public:
					CWaterEffect();

	void			UpdateSpecificConstants(const Palleon::VIEWPORT_PARAMS&, Palleon::CMaterial*, const CMatrix4&) override;

private:
	static Palleon::EffectInputBindingArray		CreateInputBindings();
	static Palleon::CShaderBuilder				CreateVertexShader();
	static Palleon::CShaderBuilder				CreatePixelShader();
};
