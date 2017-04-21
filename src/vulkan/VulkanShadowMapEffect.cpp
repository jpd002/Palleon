#include "palleon/vulkan/VulkanShadowMapEffect.h"
#include "nuanceur/generators/SpirvShaderGenerator.h"
#include "vulkan/StructDefs.h"
#include "MemStream.h"

using namespace Palleon;

CVulkanShadowMapEffect::CVulkanShadowMapEffect(Framework::Vulkan::CDevice& device)
: CVulkanEffect(device)
{
	static const auto buildShaderModule =
		[](Framework::Vulkan::CDevice& device, Nuanceur::CShaderBuilder shader, Nuanceur::CSpirvShaderGenerator::SHADER_TYPE shaderType)
		{
			auto shaderStream = Framework::CMemStream();
			Nuanceur::CSpirvShaderGenerator::Generate(shaderStream, shader, shaderType);
			shaderStream.Seek(0, Framework::STREAM_SEEK_SET);
			return Framework::Vulkan::CShaderModule(device, shaderStream);
		};
	
	CreatePipelineLayout();
	
	m_vertexShaderModule   = buildShaderModule(*m_device, BuildVertexShader(), Nuanceur::CSpirvShaderGenerator::SHADER_TYPE_VERTEX);
	m_fragmentShaderModule = buildShaderModule(*m_device, BuildFragmentShader(), Nuanceur::CSpirvShaderGenerator::SHADER_TYPE_FRAGMENT);
}

void CVulkanShadowMapEffect::UpdateConstants(const VIEWPORT_PARAMS& viewportParams, CMaterial*, const CMatrix4& worldMatrix)
{
	m_pushConstants.viewProjMatrix = viewportParams.viewMatrix * viewportParams.projMatrix;
	m_pushConstants.worldMatrix = worldMatrix;
}

void CVulkanShadowMapEffect::PrepareDraw(VkCommandBuffer commandBuffer)
{
	m_device->vkCmdPushConstants(commandBuffer, m_pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(DefaultPushConstants), &m_pushConstants);
}

void CVulkanShadowMapEffect::CreatePipelineLayout()
{
	VkResult result = VK_SUCCESS;
	
	VkPushConstantRange pushConstantInfo = {};
	pushConstantInfo.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	pushConstantInfo.offset     = 0;
	pushConstantInfo.size       = sizeof(DefaultPushConstants);
	
	auto pipelineLayoutCreateInfo = Framework::Vulkan::PipelineLayoutCreateInfo();
	pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
	pipelineLayoutCreateInfo.pPushConstantRanges    = &pushConstantInfo;
	pipelineLayoutCreateInfo.setLayoutCount         = 0;
	pipelineLayoutCreateInfo.pSetLayouts            = nullptr;
	
	result = m_device->vkCreatePipelineLayout(*m_device, &pipelineLayoutCreateInfo, nullptr, &m_pipelineLayout);
	CHECKVULKANERROR(result);
}

Nuanceur::CShaderBuilder CVulkanShadowMapEffect::BuildVertexShader()
{
	using namespace Nuanceur;
	
	auto b = CShaderBuilder();
	
	{
		auto inputPosition = CFloat4Lvalue(b.CreateInput(Nuanceur::SEMANTIC_POSITION));
		
		auto outputSystemPosition = CFloat4Lvalue(b.CreateOutput(Nuanceur::SEMANTIC_SYSTEM_POSITION));
		auto outputPosition = CFloat4Lvalue(b.CreateOutput(Nuanceur::SEMANTIC_TEXCOORD));
		
		auto viewProjMatrix = CMatrix44Value(b.CreateUniformMatrix("g_viewProjMatrix"));
		auto worldMatrix = CMatrix44Value(b.CreateUniformMatrix("g_worldMatrix"));
		
		auto worldPosition = CFloat4Lvalue(b.CreateTemporary());
		auto clipPosition = CFloat4Lvalue(b.CreateTemporary());
		
		worldPosition = worldMatrix * NewFloat4(inputPosition->xyz(), NewFloat(b, 1));
		clipPosition = viewProjMatrix * worldPosition;
		
		outputPosition = clipPosition->xyzw();
		outputSystemPosition = clipPosition->xyzw();
	}
	
	return b;
}

Nuanceur::CShaderBuilder CVulkanShadowMapEffect::BuildFragmentShader()
{
	using namespace Nuanceur;
	
	auto b = CShaderBuilder();
	
	{
		auto inputPosition = CFloat4Lvalue(b.CreateInput(Nuanceur::SEMANTIC_TEXCOORD));
		
		auto outputColor = CFloat4Lvalue(b.CreateOutput(Nuanceur::SEMANTIC_SYSTEM_COLOR));
		
		outputColor = NewFloat4(inputPosition->z() / inputPosition->w(), NewFloat3(b, 1, 1, 1));
	}
	
	return b;
}
