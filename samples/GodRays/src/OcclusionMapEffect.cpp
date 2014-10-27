#include "OcclusionMapEffect.h"

COcclusionMapEffect::COcclusionMapEffect()
: Palleon::CPlatformGenericEffect(CreateInputBindings(), CreateVertexShader(), CreatePixelShader())
{

}

Palleon::EffectInputBindingArray COcclusionMapEffect::CreateInputBindings()
{
	Palleon::EffectInputBindingArray inputBindings;
	inputBindings.push_back(Palleon::EFFECTINPUTBINDING(Palleon::VERTEX_ITEM_ID_POSITION, Palleon::SEMANTIC_POSITION, 0));
	return inputBindings;
}

Palleon::CShaderBuilder COcclusionMapEffect::CreateVertexShader()
{
	Palleon::CShaderBuilder b;

	//Inputs
	const auto inputPosition = b.CreateInputFloat3(Palleon::SEMANTIC_POSITION);

	//Outputs
	const auto outputPosition = b.CreateOutputFloat4(Palleon::SEMANTIC_SYSTEM_POSITION);
		
	//Uniforms
	const auto worldViewProjMatrix = b.CreateUniformMatrix(g_worldViewProjMatrixName);

	//Code
	auto worldPos = b.Multiply(worldViewProjMatrix, b.NewFloat4(inputPosition, b.CreateConstant(1)));
	b.Assign(outputPosition, worldPos);
	
	return b;
}

Palleon::CShaderBuilder COcclusionMapEffect::CreatePixelShader()
{
	Palleon::CShaderBuilder b;

	//Inputs
	const auto inputPosition = b.CreateInputFloat4(Palleon::SEMANTIC_SYSTEM_POSITION);

	//Outputs
	const auto outputColor = b.CreateOutputFloat4(Palleon::SEMANTIC_SYSTEM_COLOR);

	//Code
	b.Assign(outputColor, b.CreateConstant(0, 0, 0, 1));

	return b;
}
