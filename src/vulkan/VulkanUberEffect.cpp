#include "palleon/vulkan/VulkanUberEffect.h"
#include "palleon/vulkan/VulkanUberEffectGenerator.h"
#include "vulkan/StructDefs.h"
#include "MemStream.h"

using namespace Palleon;

CVulkanUberEffect::CVulkanUberEffect(Framework::Vulkan::CDevice& device, const CVulkanUberEffectGenerator::EFFECTCAPS& effectCaps)
: CVulkanEffect(device)
{
	CreatePipelineLayout();
	CreateShaderModules(effectCaps);
}

void CVulkanUberEffect::UpdateConstants(const VIEWPORT_PARAMS& viewportParams, CMaterial*, const CMatrix4& worldMatrix)
{
	m_pushConstants.viewProjMatrix = viewportParams.viewMatrix * viewportParams.projMatrix;
	m_pushConstants.worldMatrix = worldMatrix;
}

void CVulkanUberEffect::PrepareDraw(VkCommandBuffer commandBuffer)
{
	m_device->vkCmdPushConstants(commandBuffer, m_pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(DefaultPushConstants), &m_pushConstants);
}

void CVulkanUberEffect::CreatePipelineLayout()
{
	VkPushConstantRange pushConstantInfo = {};
	pushConstantInfo.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	pushConstantInfo.offset     = 0;
	pushConstantInfo.size       = sizeof(DefaultPushConstants);
	
	auto pipelineLayoutCreateInfo = Framework::Vulkan::PipelineLayoutCreateInfo();
	pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
	pipelineLayoutCreateInfo.pPushConstantRanges    = &pushConstantInfo;
	
	auto result = m_device->vkCreatePipelineLayout(*m_device, &pipelineLayoutCreateInfo, nullptr, &m_pipelineLayout);
	CHECKVULKANERROR(result);
}

void CVulkanUberEffect::CreateShaderModules(const CVulkanUberEffectGenerator::EFFECTCAPS& effectCaps)
{
	{
		auto shaderStream = Framework::CMemStream();
		CVulkanUberEffectGenerator::GenerateVertexShader(shaderStream, effectCaps);
		shaderStream.Seek(0, Framework::STREAM_SEEK_SET);
		m_vertexShaderModule = Framework::Vulkan::CShaderModule(*m_device, shaderStream);
	}
	
	{
		auto shaderStream = Framework::CMemStream();
		CVulkanUberEffectGenerator::GenerateFragmentShader(shaderStream, effectCaps);
		shaderStream.Seek(0, Framework::STREAM_SEEK_SET);
		m_fragmentShaderModule = Framework::Vulkan::CShaderModule(*m_device, shaderStream);
	}
}
