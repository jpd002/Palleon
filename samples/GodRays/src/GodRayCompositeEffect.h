#pragma once

#include "PalleonEngine.h"

class CGodRayCompositeEffect : public Palleon::CPlatformGenericEffect
{
public:
											CGodRayCompositeEffect();

private:
	static Palleon::EffectInputBindingArray	CreateInputBindings();
	static Palleon::CShaderBuilder			CreateVertexShader();
	static Palleon::CShaderBuilder			CreatePixelShader();
};
