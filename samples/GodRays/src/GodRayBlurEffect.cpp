#include "GodRayBlurEffect.h"

const char* CGodRayBlurEffect::g_screenLightPosUniformName = "p_screenLightPos";
const char* CGodRayBlurEffect::g_densityUniformName = "p_density";

CGodRayBlurEffect::CGodRayBlurEffect()
: Palleon::CPlatformGenericEffect(CreateInputBindings(), CreateVertexShader(), CreatePixelShader())
{

}

void CGodRayBlurEffect::UpdateSpecificConstants(const Palleon::VIEWPORT_PARAMS& viewportParams, Palleon::CMaterial* material, const CMatrix4& worldMatrix)
{
	SetConstant(g_screenLightPosUniformName, material->GetEffectParameter(g_screenLightPosUniformName));
	SetConstant(g_densityUniformName, material->GetEffectParameter(g_densityUniformName));
}

Palleon::EffectInputBindingArray CGodRayBlurEffect::CreateInputBindings()
{
	Palleon::EffectInputBindingArray inputBindings;
	inputBindings.push_back(Palleon::EFFECTINPUTBINDING(Palleon::VERTEX_ITEM_ID_POSITION, Palleon::SEMANTIC_POSITION, 0));
	inputBindings.push_back(Palleon::EFFECTINPUTBINDING(Palleon::VERTEX_ITEM_ID_UV0, Palleon::SEMANTIC_TEXCOORD, 0));
	return inputBindings;
}

Palleon::CShaderBuilder CGodRayBlurEffect::CreateVertexShader()
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

Palleon::CShaderBuilder CGodRayBlurEffect::CreatePixelShader()
{
	Palleon::CShaderBuilder b;

	//Inputs
	const auto inputPosition = b.CreateInputFloat4(Palleon::SEMANTIC_SYSTEM_POSITION);
	const auto inputTexCoord = b.CreateInputFloat2(Palleon::SEMANTIC_TEXCOORD, 0);

	//Outputs
	const auto outputColor = b.CreateOutputFloat4(Palleon::SEMANTIC_SYSTEM_COLOR);

	//Uniforms
	const auto texture = b.CreateTexture2D(0);
	const auto screenLightPos = b.CreateUniformFloat3(g_screenLightPosUniformName);
	const auto density = b.CreateUniformFloat(g_densityUniformName);

	static const int samples = 8;
	float invSamples = 1.f / static_cast<float>(samples);

	auto posDir = b.Substract(b.SwizzleFloat2(inputTexCoord, Palleon::SWIZZLE_XY), b.SwizzleFloat2(screenLightPos, Palleon::SWIZZLE_XY));
	auto posDelta = b.Multiply(posDir, b.CreateConstant(invSamples, invSamples));
	posDelta = b.Multiply(posDelta, b.NewFloat2(density, density));

	auto illuminationDecay = b.CreateConstant(1);

	Palleon::CShaderBuilder::SYMBOLREF_FLOAT2 texCoords[samples];
	Palleon::CShaderBuilder::SYMBOLREF_FLOAT4 texColors[samples];

	texCoords[0] = inputTexCoord;
	for(unsigned int i = 1; i < samples; i++)
	{
		texCoords[i] = b.Substract(texCoords[i - 1], posDelta);
	}

	for(unsigned int i = 0; i < samples; i++)
	{
		texColors[i] = b.Sample(texture, texCoords[i]);
		texColors[i] = b.Multiply(texColors[i], b.NewFloat4(illuminationDecay, illuminationDecay, illuminationDecay, illuminationDecay));
		illuminationDecay = b.Multiply(illuminationDecay, b.CreateConstant(0.835f));
	}

	auto colorSum = texColors[0];
	for(unsigned int i = 1; i < samples; i++)
	{
		colorSum = b.Add(texColors[i], colorSum);
	}

	colorSum = b.Multiply(colorSum, b.CreateConstant(invSamples, invSamples, invSamples, invSamples));

//	auto posDirLength = b.Length(posDir);

	b.Assign(outputColor, colorSum);

	return b;
}
