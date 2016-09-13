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

Nuanceur::CShaderBuilder CVulkanUberEffectGenerator::BuildVertexShader(const EFFECTCAPS&)
{
	using namespace Nuanceur;
	
	auto b = CShaderBuilder();
	
	{
		auto inputPosition = CFloat4Lvalue(b.CreateInput(Nuanceur::SEMANTIC_POSITION));
		
		auto outputPosition = CFloat4Lvalue(b.CreateOutput(Nuanceur::SEMANTIC_SYSTEM_POSITION));
		auto outputColor = CFloat4Lvalue(b.CreateOutput(Nuanceur::SEMANTIC_TEXCOORD));
		
		auto meshColor = CFloat4Lvalue(b.CreateUniformFloat4("g_meshColor"));
		auto viewProjMatrix = CMatrix44Value(b.CreateUniformMatrix("g_viewProjMatrix"));
		auto worldMatrix = CMatrix44Value(b.CreateUniformMatrix("g_worldMatrix"));
		
		auto worldPosition = CFloat4Lvalue(b.CreateTemporary());
		
		worldPosition = worldMatrix * NewFloat4(inputPosition->xyz(), 1);
		outputPosition = viewProjMatrix * worldPosition;
		outputColor = meshColor->xyzw();
	}
	
	return b;
}

Nuanceur::CShaderBuilder CVulkanUberEffectGenerator::BuildFragmentShader(const EFFECTCAPS&)
{
	using namespace Nuanceur;
	
	auto b = CShaderBuilder();
	
	{
		auto inputColor = CFloat4Lvalue(b.CreateInput(Nuanceur::SEMANTIC_TEXCOORD));
		
		auto outputColor = CFloat4Lvalue(b.CreateOutput(Nuanceur::SEMANTIC_SYSTEM_COLOR));
		
		outputColor = inputColor->xyzw();
	}
	
	return b;
}
