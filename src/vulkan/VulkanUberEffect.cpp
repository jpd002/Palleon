#include "palleon/vulkan/VulkanUberEffect.h"
#include "palleon/vulkan/VulkanUberEffectGenerator.h"
#include "vulkan/StructDefs.h"
#include "MemStream.h"

using namespace Palleon;

CVulkanUberEffect::CVulkanUberEffect(Framework::Vulkan::CDevice& device, const CVulkanUberEffectGenerator::EFFECTCAPS& effectCaps)
: CVulkanEffect(device)
{
	CreatePipelineLayout(effectCaps);
	CreateShaderModules(effectCaps);
}

void CVulkanUberEffect::UpdateConstants(const VIEWPORT_PARAMS& viewportParams, CMaterial* material, const CMatrix4& worldMatrix)
{
	m_pushConstants.meshColor = material->GetColor();
	m_pushConstants.viewProjMatrix = viewportParams.viewMatrix * viewportParams.projMatrix;
	m_pushConstants.worldMatrix = worldMatrix;
}

void CVulkanUberEffect::PrepareDraw(VkCommandBuffer commandBuffer)
{
	m_device->vkCmdPushConstants(commandBuffer, m_pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(DefaultPushConstants), &m_pushConstants);
}

void CVulkanUberEffect::CreatePipelineLayout(const CVulkanUberEffectGenerator::EFFECTCAPS& effectCaps)
{
	VkResult result = VK_SUCCESS;
	
	if(effectCaps.hasTexture)
	{
		VkDescriptorSetLayoutBinding setLayoutBinding = {};
		setLayoutBinding.binding         = 0;
		setLayoutBinding.descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		setLayoutBinding.descriptorCount = 1;
		setLayoutBinding.stageFlags      = VK_SHADER_STAGE_FRAGMENT_BIT;
		
		auto setLayoutCreateInfo = Framework::Vulkan::DescriptorSetLayoutCreateInfo();
		setLayoutCreateInfo.bindingCount = 1;
		setLayoutCreateInfo.pBindings    = &setLayoutBinding;
		
		result = m_device->vkCreateDescriptorSetLayout(*m_device, &setLayoutCreateInfo, nullptr, &m_descriptorSetLayout);
		CHECKVULKANERROR(result);
	}
	
	VkPushConstantRange pushConstantInfo = {};
	pushConstantInfo.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	pushConstantInfo.offset     = 0;
	pushConstantInfo.size       = sizeof(DefaultPushConstants);
	
	auto pipelineLayoutCreateInfo = Framework::Vulkan::PipelineLayoutCreateInfo();
	pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
	pipelineLayoutCreateInfo.pPushConstantRanges    = &pushConstantInfo;
	pipelineLayoutCreateInfo.setLayoutCount         = (m_descriptorSetLayout == VK_NULL_HANDLE) ? 0 : 1;
	pipelineLayoutCreateInfo.pSetLayouts            = &m_descriptorSetLayout;
	
	result = m_device->vkCreatePipelineLayout(*m_device, &pipelineLayoutCreateInfo, nullptr, &m_pipelineLayout);
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
