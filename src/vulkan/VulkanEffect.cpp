#include "palleon/vulkan/VulkanEffect.h"
#include "vulkan/StructDefs.h"

using namespace Palleon;

CVulkanEffect::CVulkanEffect(Framework::Vulkan::CDevice& device)
: m_device(&device)
{
	
}

CVulkanEffect::~CVulkanEffect()
{
	for(const auto& pipelinePair : m_pipelines)
	{
		m_device->vkDestroyPipeline(*m_device, pipelinePair.second, nullptr);
	}
	if(m_pipelineLayout != VK_NULL_HANDLE)
	{
		m_device->vkDestroyPipelineLayout(*m_device, m_pipelineLayout, nullptr);
	}
}

VkPipeline CVulkanEffect::GetPipelineForMesh(CMesh* mesh, VkRenderPass renderPass)
{
	assert(!m_vertexShaderModule.IsEmpty());
	assert(!m_fragmentShaderModule.IsEmpty());
	assert(m_pipelineLayout != VK_NULL_HANDLE);
	
	const auto& vertexBufferDescriptor = mesh->GetVertexBuffer()->GetDescriptor();
	
	VkPipeline pipeline = VK_NULL_HANDLE;
	
	VULKAN_PIPELINE_KEY pipelineKey;
	pipelineKey.renderPass    = renderPass;
	pipelineKey.primitiveType = mesh->GetPrimitiveType();
	pipelineKey.vertexItems   = vertexBufferDescriptor.vertexItems;
	
	auto pipelineIterator = m_pipelines.find(pipelineKey);
	if(pipelineIterator != std::end(m_pipelines))
	{
		return pipelineIterator->second;
	}
	
	auto inputAssemblyInfo = Framework::Vulkan::PipelineInputAssemblyStateCreateInfo();
	switch(mesh->GetPrimitiveType())
	{
	case PRIMITIVE_TRIANGLE_LIST:
		inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		break;
	case PRIMITIVE_TRIANGLE_STRIP:
		inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
		break;
	}
	
	std::vector<VkVertexInputAttributeDescription> attributeDescs;
	for(const auto& vertexItem : vertexBufferDescriptor.vertexItems)
	{
		if(vertexItem.id == VERTEX_ITEM_ID_NONE) continue;
		
		VkVertexInputAttributeDescription attributeDesc = {};
		attributeDesc.binding = 0;
		attributeDesc.offset = vertexItem.offset;
		
		//TODO: Use MapSemanticToLocation here
		switch(vertexItem.id)
		{
		case VERTEX_ITEM_ID_POSITION:
			attributeDesc.location = 0;
			break;
		case VERTEX_ITEM_ID_UV0:
			attributeDesc.location = 1;
			break;
		case VERTEX_ITEM_ID_NORMAL:
			attributeDesc.location = 2;
			break;
		default:
			assert(false);
			break;
		}
		
		switch(vertexItem.size)
		{
		case 8:
			attributeDesc.format = VK_FORMAT_R32G32_SFLOAT;
			break;
		case 12:
			attributeDesc.format = VK_FORMAT_R32G32B32_SFLOAT;
			break;
		default:
			assert(false);
			break;
		}
		
		attributeDescs.push_back(attributeDesc);
	}
	
	VkVertexInputBindingDescription binding = {};
	binding.binding   = 0;
	binding.stride    = vertexBufferDescriptor.GetVertexSize();
	binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	
	auto vertexInputInfo = Framework::Vulkan::PipelineVertexInputStateCreateInfo();
	vertexInputInfo.vertexBindingDescriptionCount   = 1;
	vertexInputInfo.pVertexBindingDescriptions      = &binding;
	vertexInputInfo.vertexAttributeDescriptionCount = attributeDescs.size();
	vertexInputInfo.pVertexAttributeDescriptions    = attributeDescs.data();

	auto rasterStateInfo = Framework::Vulkan::PipelineRasterizationStateCreateInfo();
	rasterStateInfo.polygonMode             = VK_POLYGON_MODE_FILL;
	rasterStateInfo.cullMode                = VK_CULL_MODE_NONE;
	rasterStateInfo.frontFace               = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterStateInfo.depthClampEnable        = false;
	rasterStateInfo.rasterizerDiscardEnable = false;
	rasterStateInfo.depthBiasEnable         = false;
	rasterStateInfo.lineWidth               = 1.0f;
	
	// Our attachment will write to all color channels, but no blending is enabled.
	VkPipelineColorBlendAttachmentState blendAttachment = { 0 };
	blendAttachment.blendEnable    = false;
	blendAttachment.colorWriteMask = 0xf;
	
	auto colorBlendStateInfo = Framework::Vulkan::PipelineColorBlendStateCreateInfo();
	colorBlendStateInfo.attachmentCount = 1;
	colorBlendStateInfo.pAttachments    = &blendAttachment;
	
	auto viewportStateInfo = Framework::Vulkan::PipelineViewportStateCreateInfo();
	viewportStateInfo.viewportCount = 1;
	viewportStateInfo.scissorCount  = 1;
	
	auto depthStencilStateInfo = Framework::Vulkan::PipelineDepthStencilStateCreateInfo();
	depthStencilStateInfo.depthTestEnable       = false;
	depthStencilStateInfo.depthWriteEnable      = false;
	depthStencilStateInfo.depthBoundsTestEnable = false;
	depthStencilStateInfo.stencilTestEnable     = false;
	
	auto multisampleStateInfo = Framework::Vulkan::PipelineMultisampleStateCreateInfo();
	multisampleStateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	static const VkDynamicState dynamicStates[] = 
	{
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR,
	};
	auto dynamicStateInfo = Framework::Vulkan::PipelineDynamicStateCreateInfo();
	dynamicStateInfo.pDynamicStates    = dynamicStates;
	dynamicStateInfo.dynamicStateCount = sizeof(dynamicStates) / sizeof(dynamicStates[0]);
	
	VkPipelineShaderStageCreateInfo shaderStages[2] =
	{
		{ VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO },
		{ VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO },
	};
	
	shaderStages[0].stage  = VK_SHADER_STAGE_VERTEX_BIT;
	shaderStages[0].module = m_vertexShaderModule;
	shaderStages[0].pName  = "main";
	shaderStages[1].stage  = VK_SHADER_STAGE_FRAGMENT_BIT;
	shaderStages[1].module = m_fragmentShaderModule;
	shaderStages[1].pName  = "main";

	auto pipelineCreateInfo = Framework::Vulkan::GraphicsPipelineCreateInfo();
	pipelineCreateInfo.stageCount          = 2;
	pipelineCreateInfo.pStages             = shaderStages;
	pipelineCreateInfo.pInputAssemblyState = &inputAssemblyInfo;
	pipelineCreateInfo.pVertexInputState   = &vertexInputInfo;
	pipelineCreateInfo.pRasterizationState = &rasterStateInfo;
	pipelineCreateInfo.pColorBlendState    = &colorBlendStateInfo;
	pipelineCreateInfo.pViewportState      = &viewportStateInfo;
	pipelineCreateInfo.pDepthStencilState  = &depthStencilStateInfo;
	pipelineCreateInfo.pMultisampleState   = &multisampleStateInfo;
	pipelineCreateInfo.pDynamicState       = &dynamicStateInfo;
	pipelineCreateInfo.renderPass          = renderPass;
	pipelineCreateInfo.layout              = m_pipelineLayout;
	
	auto result = m_device->vkCreateGraphicsPipelines(*m_device, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &pipeline);
	CHECKVULKANERROR(result);
	
	m_pipelines.insert(std::make_pair(pipelineKey, pipeline));
	
	return pipeline;
}
