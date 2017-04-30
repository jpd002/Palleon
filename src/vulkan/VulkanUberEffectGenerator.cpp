#include "palleon/vulkan/VulkanUberEffectGenerator.h"
#include "nuanceur/generators/SpirvShaderGenerator.h"

using namespace Palleon;

void CVulkanUberEffectGenerator::GenerateVertexShader(Framework::CStream& outputStream, const EFFECTCAPS& caps)
{
	auto vertexShader = BuildVertexShader(caps);
	Nuanceur::CSpirvShaderGenerator::Generate(outputStream, vertexShader, Nuanceur::CSpirvShaderGenerator::SHADER_TYPE_VERTEX);
}

void CVulkanUberEffectGenerator::GenerateFragmentShader(Framework::CStream& outputStream, const EFFECTCAPS& caps)
{
	auto fragmentShader = BuildFragmentShader(caps);
	Nuanceur::CSpirvShaderGenerator::Generate(outputStream, fragmentShader, Nuanceur::CSpirvShaderGenerator::SHADER_TYPE_FRAGMENT);
}

Nuanceur::CShaderBuilder CVulkanUberEffectGenerator::BuildVertexShader(const EFFECTCAPS& caps)
{
	using namespace Nuanceur;
	
	auto b = CShaderBuilder();
	
	{
		auto inputPosition = CFloat4Lvalue(b.CreateInput(Nuanceur::SEMANTIC_POSITION));
		auto inputTexCoord = CFloat4Lvalue(b.CreateOptionalInput(caps.hasTexture, Nuanceur::SEMANTIC_TEXCOORD, 0));
		
		auto outputPosition = CFloat4Lvalue(b.CreateOutput(Nuanceur::SEMANTIC_SYSTEM_POSITION));
		auto outputColor = CFloat4Lvalue(b.CreateOutput(Nuanceur::SEMANTIC_TEXCOORD, 0));
		auto outputTexCoord = CFloat4Lvalue(b.CreateOutput(Nuanceur::SEMANTIC_TEXCOORD, 1));
		auto outputShadowPosition = CFloat4Lvalue(b.CreateOptionalOutput(caps.hasShadowMap, Nuanceur::SEMANTIC_TEXCOORD, 2));
		
		auto meshColor = CFloat4Lvalue(b.CreateUniformFloat4("g_meshColor"));
		auto viewProjMatrix = CMatrix44Value(b.CreateUniformMatrix("g_viewProjMatrix"));
		auto worldMatrix = CMatrix44Value(b.CreateUniformMatrix("g_worldMatrix"));
		auto shadowViewProjMatrix = CMatrix44Value(b.CreateOptionalUniformMatrix(caps.hasShadowMap, "g_shadowViewProjMatrix"));
		
		auto worldPosition = CFloat4Lvalue(b.CreateTemporary());
		
		worldPosition = worldMatrix * NewFloat4(inputPosition->xyz(), NewFloat(b, 1));
		outputPosition = viewProjMatrix * worldPosition;
		outputColor = meshColor->xyzw();
		
		if(caps.hasTexture)
		{
			outputTexCoord = inputTexCoord->xyzw();
		}
		
		if(caps.hasShadowMap)
		{
			outputShadowPosition = shadowViewProjMatrix * worldPosition;
		}
	}
	
	return b;
}

Nuanceur::CShaderBuilder CVulkanUberEffectGenerator::BuildFragmentShader(const EFFECTCAPS& caps)
{
	using namespace Nuanceur;
	
	auto b = CShaderBuilder();
	
	{
		auto inputColor = CFloat4Lvalue(b.CreateInput(Nuanceur::SEMANTIC_TEXCOORD, 0));
		auto inputTexCoord = CFloat4Lvalue(b.CreateInput(Nuanceur::SEMANTIC_TEXCOORD, 1));
		auto inputShadowPosition = CFloat4Lvalue(b.CreateOptionalInput(caps.hasShadowMap, Nuanceur::SEMANTIC_TEXCOORD, 2));
		
		auto outputColor = CFloat4Lvalue(b.CreateOutput(Nuanceur::SEMANTIC_SYSTEM_COLOR));
		
		outputColor = inputColor->xyzw();
		
		if(caps.hasTexture)
		{
			auto texture = CTexture2DValue(b.CreateTexture2D(DESCRIPTOR_BINDING_DIFFUSE));
			outputColor = Sample(texture, inputTexCoord->xy());
		}
		
		if(caps.hasShadowMap)
		{
			auto shadowMap = CTexture2DValue(b.CreateTexture2D(DESCRIPTOR_BINDING_SHADOWMAP));
			auto shadowMapCoord = CFloat2Lvalue(b.CreateTemporary());
			auto shadowZ = CFloatLvalue(b.CreateTemporary());
			auto pixelZ = CFloatLvalue(b.CreateTemporary());
			
			shadowMapCoord = inputShadowPosition->xy() / inputShadowPosition->ww();
			shadowMapCoord = NewFloat2(b, 0.5f, 0.5f) * shadowMapCoord + NewFloat2(b, 0.5f, 0.5f);
			shadowZ = Sample(shadowMap, shadowMapCoord)->x();
			pixelZ = inputShadowPosition->z() / inputShadowPosition->w();
			BeginIf(b, shadowZ < pixelZ);
			{
				outputColor = outputColor * NewFloat4(b, 0.25f, 0.25f, 0.25f, 1);
			}
			EndIf(b);
		}
	}
	
	return b;
}
