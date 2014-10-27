#pragma once

#include "PalleonEngine.h"

class CGodRayBlurEffect : public Palleon::CPlatformGenericEffect
{
public:
											CGodRayBlurEffect();

	static const char*						g_screenLightPosUniformName;
	static const char*						g_densityUniformName;

	void									UpdateSpecificConstants(const Palleon::VIEWPORT_PARAMS&, Palleon::CMaterial*, const CMatrix4&) override;

private:
	static Palleon::EffectInputBindingArray	CreateInputBindings();
	static Palleon::CShaderBuilder			CreateVertexShader();
	static Palleon::CShaderBuilder			CreatePixelShader();
};
