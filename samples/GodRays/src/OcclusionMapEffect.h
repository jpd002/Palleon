#pragma once

#include "PalleonEngine.h"

class COcclusionMapEffect : public Palleon::CPlatformGenericEffect
{
public:
											COcclusionMapEffect();

private:
	static Palleon::EffectInputBindingArray	CreateInputBindings();
	static Palleon::CShaderBuilder			CreateVertexShader();
	static Palleon::CShaderBuilder			CreatePixelShader();
};
