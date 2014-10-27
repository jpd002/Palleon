#include "GodRayCompositeEffect.h"

CGodRayCompositeEffect::CGodRayCompositeEffect()
: Palleon::CPlatformGenericEffect(CreateInputBindings(), CreateVertexShader(), CreatePixelShader())
{

}

Palleon::EffectInputBindingArray CGodRayCompositeEffect::CreateInputBindings()
{
	Palleon::EffectInputBindingArray inputBindings;
	inputBindings.push_back(Palleon::EFFECTINPUTBINDING(Palleon::VERTEX_ITEM_ID_POSITION, Palleon::SEMANTIC_POSITION, 0));
	inputBindings.push_back(Palleon::EFFECTINPUTBINDING(Palleon::VERTEX_ITEM_ID_UV0, Palleon::SEMANTIC_TEXCOORD, 0));
	return inputBindings;
}

Palleon::CShaderBuilder CGodRayCompositeEffect::CreateVertexShader()
{
	Palleon::CShaderBuilder b;

	//Inputs
	const auto inputPosition = b.CreateInputFloat3(Palleon::SEMANTIC_POSITION);
	const auto inputTexCoord = b.CreateInputFloat2(Palleon::SEMANTIC_TEXCOORD);

	//Outputs
	const auto outputPosition = b.CreateOutputFloat4(Palleon::SEMANTIC_SYSTEM_POSITION);
	const auto outputTexCoord = b.CreateOutputFloat2(Palleon::SEMANTIC_TEXCOORD, 0);

	//Uniforms
	b.Assign(outputPosition, b.NewFloat4(inputPosition, b.CreateConstant(1)));
	b.Assign(outputTexCoord, inputTexCoord);

	return b;
}

Palleon::CShaderBuilder CGodRayCompositeEffect::CreatePixelShader()
{
	Palleon::CShaderBuilder b;

	//Inputs
	const auto inputPosition = b.CreateInputFloat4(Palleon::SEMANTIC_SYSTEM_POSITION);
	const auto inputTexCoord = b.CreateInputFloat2(Palleon::SEMANTIC_TEXCOORD, 0);

	//Outputs
	const auto outputColor = b.CreateOutputFloat4(Palleon::SEMANTIC_SYSTEM_COLOR);

	//Uniforms
	const auto sceneTexture = b.CreateTexture2D(0);
	const auto godRayTexture = b.CreateTexture2D(1);

	auto sceneTextureColor = b.Sample(sceneTexture, inputTexCoord);
	auto godRayTextureColor = b.Sample(godRayTexture, inputTexCoord);

//	auto finalColor = b.Mix(sceneTextureColor, godRayTextureColor, b.SwizzleFloat(godRayTextureColor, Palleon::SWIZZLE_W));
	auto occlusionFactor = b.SwizzleFloat(godRayTextureColor, Palleon::SWIZZLE_W);
	float intensity = 6;

	godRayTextureColor = b.Multiply(godRayTextureColor, b.NewFloat4(occlusionFactor, occlusionFactor, occlusionFactor, occlusionFactor));
	godRayTextureColor = b.Multiply(godRayTextureColor, b.CreateConstant(intensity, intensity, intensity, intensity));
	auto finalColor = b.Add(sceneTextureColor, godRayTextureColor);
	b.Assign(outputColor, finalColor);

	return b;
}
