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
		
		auto meshColor = CFloat4Lvalue(b.CreateUniformFloat4("g_meshColor"));
		auto viewProjMatrix = CMatrix44Value(b.CreateUniformMatrix("g_viewProjMatrix"));
		auto worldMatrix = CMatrix44Value(b.CreateUniformMatrix("g_worldMatrix"));
		
		auto worldPosition = CFloat4Lvalue(b.CreateTemporary());
		
		worldPosition = worldMatrix * NewFloat4(inputPosition->xyz(), 1);
		outputPosition = viewProjMatrix * worldPosition;
		outputColor = meshColor->xyzw();
		
		if(caps.hasTexture)
		{
			outputTexCoord = inputTexCoord->xyzw();
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
		
		auto outputColor = CFloat4Lvalue(b.CreateOutput(Nuanceur::SEMANTIC_SYSTEM_COLOR));
		
		outputColor = inputColor->xyzw();
		
		if(caps.hasTexture)
		{
			auto texture = CTexture2DValue(b.CreateTexture2D(0));
			outputColor = Sample(texture, inputTexCoord->xy());
		}
	}
	
	return b;
}
