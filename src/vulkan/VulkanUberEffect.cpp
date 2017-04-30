#include "palleon/vulkan/VulkanUberEffect.h"
#include "palleon/vulkan/VulkanUberEffectGenerator.h"
#include "palleon/graphics/OffsetKeeper.h"
#include "vulkan/StructDefs.h"
#include "MemStream.h"

using namespace Palleon;

CVulkanUberEffect::CVulkanUberEffect(Framework::Vulkan::CDevice& device, const CVulkanUberEffectGenerator::EFFECTCAPS& effectCaps)
: CVulkanEffect(device)
{
	{
		OffsetKeeper constantOffset;
		
		m_meshColorOffset      = constantOffset.Allocate(0x10);
		m_viewProjMatrixOffset = constantOffset.Allocate(0x40);
		m_worldMatrixOffset    = constantOffset.Allocate(0x40);
		
		if(effectCaps.hasShadowMap) m_shadowViewProjMatrixOffset = constantOffset.Allocate(0x40);
		
		m_vertexConstantBuffer.resize(constantOffset.currentOffset);
	}
	
	CreatePipelineLayout(effectCaps);
	CreateShaderModules(effectCaps);
}

void CVulkanUberEffect::UpdateConstants(const VIEWPORT_PARAMS& viewportParams, CMaterial* material, const CMatrix4& worldMatrix)
{
	if(m_meshColorOffset != -1)
	{
		*reinterpret_cast<CColor*>(m_vertexConstantBuffer.data() + m_meshColorOffset) = material->GetColor();
	}
	if(m_viewProjMatrixOffset != -1)
	{
		*reinterpret_cast<CMatrix4*>(m_vertexConstantBuffer.data() + m_viewProjMatrixOffset) = viewportParams.viewMatrix * viewportParams.projMatrix;
	}
	if(m_worldMatrixOffset != -1)
	{
		*reinterpret_cast<CMatrix4*>(m_vertexConstantBuffer.data() + m_worldMatrixOffset) = worldMatrix;
	}
	if(m_shadowViewProjMatrixOffset != -1)
	{
		*reinterpret_cast<CMatrix4*>(m_vertexConstantBuffer.data() + m_shadowViewProjMatrixOffset) = viewportParams.shadowViewProjMatrix;
	}
}

void CVulkanUberEffect::PrepareDraw(VkCommandBuffer commandBuffer)
{
	m_device->vkCmdPushConstants(commandBuffer, m_pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, m_vertexConstantBuffer.size(), m_vertexConstantBuffer.data());
}

void CVulkanUberEffect::CreatePipelineLayout(const CVulkanUberEffectGenerator::EFFECTCAPS& effectCaps)
{
	VkResult result = VK_SUCCESS;
	
	std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings;
	
	if(effectCaps.hasTexture)
	{
		VkDescriptorSetLayoutBinding setLayoutBinding = {};
		setLayoutBinding.binding         = CVulkanUberEffectGenerator::DESCRIPTOR_BINDING_DIFFUSE;
		setLayoutBinding.descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		setLayoutBinding.descriptorCount = 1;
		setLayoutBinding.stageFlags      = VK_SHADER_STAGE_FRAGMENT_BIT;
		setLayoutBindings.push_back(setLayoutBinding);
	}
	
	if(effectCaps.hasShadowMap)
	{
		VkDescriptorSetLayoutBinding setLayoutBinding = {};
		setLayoutBinding.binding         = CVulkanUberEffectGenerator::DESCRIPTOR_BINDING_SHADOWMAP;
		setLayoutBinding.descriptorType  = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		setLayoutBinding.descriptorCount = 1;
		setLayoutBinding.stageFlags      = VK_SHADER_STAGE_FRAGMENT_BIT;
		setLayoutBindings.push_back(setLayoutBinding);
	}
	
	if(!setLayoutBindings.empty())
	{
		auto setLayoutCreateInfo = Framework::Vulkan::DescriptorSetLayoutCreateInfo();
		setLayoutCreateInfo.bindingCount = setLayoutBindings.size();
		setLayoutCreateInfo.pBindings    = setLayoutBindings.data();
		
		result = m_device->vkCreateDescriptorSetLayout(*m_device, &setLayoutCreateInfo, nullptr, &m_descriptorSetLayout);
		CHECKVULKANERROR(result);
	}
	
	VkPushConstantRange pushConstantInfo = {};
	pushConstantInfo.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	pushConstantInfo.offset     = 0;
	pushConstantInfo.size       = m_vertexConstantBuffer.size();
	
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
