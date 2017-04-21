#include "palleon/vulkan/VulkanShadowMapRenderer.h"
#include "palleon/vulkan/VulkanVertexBuffer.h"
#include "palleon/vulkan/VulkanUtils.h"
#include "palleon/graphics/Mesh.h"
#include "palleon/graphics/Viewport.h"
#include "palleon/Log.h"
#include "vulkan/StructDefs.h"
#include "countof.h"

#define SHADOW_MAP_SIZE         2048
#define SHADOW_MAP_FORMAT       VK_FORMAT_R32_SFLOAT
#define SHADOW_MAP_DEPTH_FORMAT VK_FORMAT_D16_UNORM

using namespace Palleon;

CVulkanShadowMapRenderer::CVulkanShadowMapRenderer(Framework::Vulkan::CDevice& device, const VkPhysicalDeviceMemoryProperties& memoryProperties)
: m_device(&device)
, m_effect(device)
{
	m_shadowMapRenderPass = CreateRenderPass(SHADOW_MAP_FORMAT, SHADOW_MAP_DEPTH_FORMAT, false);
	CreateMap(memoryProperties);
	CreateDepth(memoryProperties);
	CreateFramebuffer();
}

CVulkanShadowMapRenderer::~CVulkanShadowMapRenderer()
{
	m_device->vkDestroyFramebuffer(*m_device, m_shadowMapFramebuffer, nullptr);

	m_device->vkDestroyImageView(*m_device, m_shadowMapDepthImageView, nullptr);
	m_device->vkFreeMemory(*m_device, m_shadowMapDepthMemory, nullptr);
	m_device->vkDestroyImage(*m_device, m_shadowMapDepthImage, nullptr);

	m_device->vkDestroyImageView(*m_device, m_shadowMapImageView, nullptr);
	m_device->vkFreeMemory(*m_device, m_shadowMapMemory, nullptr);
	m_device->vkDestroyImage(*m_device, m_shadowMapImage, nullptr);
	
	m_device->vkDestroyRenderPass(*m_device, m_shadowMapRenderPass, nullptr);
}

VkImageView CVulkanShadowMapRenderer::GetImageView() const
{
	return m_shadowMapImageView;
}

VkRenderPass CVulkanShadowMapRenderer::CreateRenderPass(VkFormat colorFormat, VkFormat depthFormat, bool preserveColor)
{
	assert(!m_device->IsEmpty());
	
	auto result = VK_SUCCESS;
	
	VkAttachmentDescription colorAttachment = {};
	colorAttachment.format         = colorFormat;
	colorAttachment.samples        = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp         = preserveColor ? VK_ATTACHMENT_LOAD_OP_LOAD : VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.initialLayout  = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	colorAttachment.finalLayout    = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	
	VkAttachmentDescription depthAttachment = {};
	depthAttachment.format         = depthFormat;
	depthAttachment.samples        = VK_SAMPLE_COUNT_1_BIT;
	depthAttachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp        = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout  = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	depthAttachment.finalLayout    = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	
	VkAttachmentDescription attachments[] =
	{
		colorAttachment,
		depthAttachment
	};
	
	VkAttachmentReference colorRef = {};
	colorRef.attachment = 0;
	colorRef.layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	
	VkAttachmentReference depthRef = {};
	depthRef.attachment = 1;
	depthRef.layout     = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	
	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount    = 1;
	subpass.pColorAttachments       = &colorRef;
	subpass.pDepthStencilAttachment = &depthRef;
	 
	auto renderPassCreateInfo = Framework::Vulkan::RenderPassCreateInfo();
	renderPassCreateInfo.attachmentCount = countof(attachments);
	renderPassCreateInfo.pAttachments    = attachments;
	renderPassCreateInfo.subpassCount    = 1;
	renderPassCreateInfo.pSubpasses      = &subpass;
	
	VkRenderPass renderPass = VK_NULL_HANDLE;
	result = m_device->vkCreateRenderPass(*m_device, &renderPassCreateInfo, nullptr, &renderPass);
	CHECKVULKANERROR(result);
	
	return renderPass;
}

void CVulkanShadowMapRenderer::CreateMap(const VkPhysicalDeviceMemoryProperties& memoryProperties)
{
	VkResult result = VK_SUCCESS;
	
	assert(!m_device->IsEmpty());
	
	{
		auto imageCreateInfo = Framework::Vulkan::ImageCreateInfo();
		imageCreateInfo.imageType     = VK_IMAGE_TYPE_2D;
		imageCreateInfo.format        = SHADOW_MAP_FORMAT;
		imageCreateInfo.extent.width  = SHADOW_MAP_SIZE;
		imageCreateInfo.extent.height = SHADOW_MAP_SIZE;
		imageCreateInfo.extent.depth  = 1;
		imageCreateInfo.mipLevels     = 1;
		imageCreateInfo.arrayLayers   = 1;
		imageCreateInfo.samples       = VK_SAMPLE_COUNT_1_BIT;
		imageCreateInfo.sharingMode   = VK_SHARING_MODE_EXCLUSIVE;
		imageCreateInfo.tiling        = VK_IMAGE_TILING_OPTIMAL;
		imageCreateInfo.usage         = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		
		result = m_device->vkCreateImage(*m_device, &imageCreateInfo, nullptr, &m_shadowMapImage);
		CHECKVULKANERROR(result);
	}
	
	VkMemoryRequirements memoryRequirements = {};
	m_device->vkGetImageMemoryRequirements(*m_device, m_shadowMapImage, &memoryRequirements);
	
	auto memoryTypeIndex = GetMemoryTypeIndex(memoryProperties, memoryRequirements.memoryTypeBits, 0);
	assert(memoryTypeIndex != VULKAN_MEMORY_TYPE_INVALID);
	
	auto memoryAllocateInfo = Framework::Vulkan::MemoryAllocateInfo();
	memoryAllocateInfo.allocationSize  = memoryRequirements.size;
	memoryAllocateInfo.memoryTypeIndex = memoryTypeIndex;
	
	result = m_device->vkAllocateMemory(*m_device, &memoryAllocateInfo, nullptr, &m_shadowMapMemory);
	CHECKVULKANERROR(result);
	
	result = m_device->vkBindImageMemory(*m_device, m_shadowMapImage, m_shadowMapMemory, 0);
	CHECKVULKANERROR(result);
	
	{
		auto imageViewCreateInfo = Framework::Vulkan::ImageViewCreateInfo();
		imageViewCreateInfo.image      = m_shadowMapImage;
		imageViewCreateInfo.viewType   = VK_IMAGE_VIEW_TYPE_2D;
		imageViewCreateInfo.format     = SHADOW_MAP_FORMAT;
		imageViewCreateInfo.components =
		{ 
			VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, 
			VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A 
		};
		imageViewCreateInfo.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
		
		result = m_device->vkCreateImageView(*m_device, &imageViewCreateInfo, nullptr, &m_shadowMapImageView);
		CHECKVULKANERROR(result);
	}
}

void CVulkanShadowMapRenderer::CreateDepth(const VkPhysicalDeviceMemoryProperties& memoryProperties)
{
	VkResult result = VK_SUCCESS;
	
	{
		auto imageCreateInfo = Framework::Vulkan::ImageCreateInfo();
		imageCreateInfo.imageType     = VK_IMAGE_TYPE_2D;
		imageCreateInfo.format        = SHADOW_MAP_DEPTH_FORMAT;
		imageCreateInfo.extent.width  = SHADOW_MAP_SIZE;
		imageCreateInfo.extent.height = SHADOW_MAP_SIZE;
		imageCreateInfo.extent.depth  = 1;
		imageCreateInfo.mipLevels     = 1;
		imageCreateInfo.arrayLayers   = 1;
		imageCreateInfo.samples       = VK_SAMPLE_COUNT_1_BIT;
		imageCreateInfo.sharingMode   = VK_SHARING_MODE_EXCLUSIVE;
		imageCreateInfo.tiling        = VK_IMAGE_TILING_OPTIMAL;
		imageCreateInfo.usage         = VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
		imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		
		result = m_device->vkCreateImage(*m_device, &imageCreateInfo, nullptr, &m_shadowMapDepthImage);
		CHECKVULKANERROR(result);
	}
	
	VkMemoryRequirements memoryRequirements = {};
	m_device->vkGetImageMemoryRequirements(*m_device, m_shadowMapDepthImage, &memoryRequirements);
	
	auto memoryTypeIndex = GetMemoryTypeIndex(memoryProperties,
		memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT);
	if(memoryTypeIndex == VULKAN_MEMORY_TYPE_INVALID)
	{
		CLog::GetInstance().Print("Lazily allocated memory not available for depth buffer, fallback to device local memory.");
		memoryTypeIndex = GetMemoryTypeIndex(memoryProperties,
			memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
	}
	assert(memoryTypeIndex != VULKAN_MEMORY_TYPE_INVALID);
	
	auto memoryAllocateInfo = Framework::Vulkan::MemoryAllocateInfo();
	memoryAllocateInfo.allocationSize  = memoryRequirements.size;
	memoryAllocateInfo.memoryTypeIndex = memoryTypeIndex;
	
	result = m_device->vkAllocateMemory(*m_device, &memoryAllocateInfo, nullptr, &m_shadowMapDepthMemory);
	CHECKVULKANERROR(result);
	
	result = m_device->vkBindImageMemory(*m_device, m_shadowMapDepthImage, m_shadowMapDepthMemory, 0);
	CHECKVULKANERROR(result);
	
	{
		auto imageViewCreateInfo = Framework::Vulkan::ImageViewCreateInfo();
		imageViewCreateInfo.image      = m_shadowMapDepthImage;
		imageViewCreateInfo.viewType   = VK_IMAGE_VIEW_TYPE_2D;
		imageViewCreateInfo.format     = SHADOW_MAP_DEPTH_FORMAT;
		imageViewCreateInfo.components =
		{ 
			VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, 
			VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A 
		};
		imageViewCreateInfo.subresourceRange = { VK_IMAGE_ASPECT_DEPTH_BIT, 0, 1, 0, 1 };
		
		result = m_device->vkCreateImageView(*m_device, &imageViewCreateInfo, nullptr, &m_shadowMapDepthImageView);
		CHECKVULKANERROR(result);
	}
}

void CVulkanShadowMapRenderer::CreateFramebuffer()
{
	VkImageView attachments[] =
	{
		m_shadowMapImageView,
		m_shadowMapDepthImageView
	};
	
	auto frameBufferCreateInfo = Framework::Vulkan::FramebufferCreateInfo();
	frameBufferCreateInfo.renderPass      = m_shadowMapRenderPass;
	frameBufferCreateInfo.attachmentCount = countof(attachments);
	frameBufferCreateInfo.pAttachments    = attachments;
	frameBufferCreateInfo.width           = SHADOW_MAP_SIZE;
	frameBufferCreateInfo.height          = SHADOW_MAP_SIZE;
	frameBufferCreateInfo.layers          = 1;
	
	VkFramebuffer framebuffer = VK_NULL_HANDLE;
	auto result = m_device->vkCreateFramebuffer(*m_device, &frameBufferCreateInfo, nullptr, &m_shadowMapFramebuffer);
	CHECKVULKANERROR(result);
}

void CVulkanShadowMapRenderer::DrawViewport(VkCommandBuffer commandBuffer, CViewport* viewport)
{
	VkResult result = VK_SUCCESS;
	
	RenderQueue renderQueue;

	auto camera = viewport->GetShadowCamera();
	if(!camera) return;
	
	const auto& sceneRoot = viewport->GetSceneRoot();
	sceneRoot->TraverseNodes(
		[&] (const SceneNodePtr& node)
		{
			if(!node->GetVisible()) return false;
			
			if(auto mesh = std::dynamic_pointer_cast<CMesh>(node))
			{
				if(
					(mesh->GetPrimitiveCount() != 0) &&
					(mesh->GetMaterial()->GetShadowCasting())
				)
				{
					//Frustum culling?
					renderQueue.push_back(mesh.get());
				}
			}
			
			return true;
		}
	);
	
	{
		auto imageMemoryBarrier = Framework::Vulkan::ImageMemoryBarrier();
		imageMemoryBarrier.image               = m_shadowMapImage;
		imageMemoryBarrier.oldLayout           = VK_IMAGE_LAYOUT_UNDEFINED;
		imageMemoryBarrier.newLayout           = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		imageMemoryBarrier.srcAccessMask       = 0;
		imageMemoryBarrier.dstAccessMask       = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imageMemoryBarrier.subresourceRange    = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
		
		m_device->vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT, VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
			0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);
	}
	
	//Transition our depth buffer
	{
		auto imageMemoryBarrier = Framework::Vulkan::ImageMemoryBarrier();
		imageMemoryBarrier.image               = m_shadowMapDepthImage;
		imageMemoryBarrier.srcAccessMask       = 0;
		imageMemoryBarrier.dstAccessMask       = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		imageMemoryBarrier.oldLayout           = VK_IMAGE_LAYOUT_UNDEFINED;
		imageMemoryBarrier.newLayout           = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imageMemoryBarrier.subresourceRange    = { VK_IMAGE_ASPECT_DEPTH_BIT, 0, 1, 0, 1 };
		
		m_device->vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT, VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
			0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);
	}
	
	VkClearValue colorClearValue;
	colorClearValue.color = { { 1.0f, 0.0f, 0.0f, 1.0f } };
	
	VkClearValue depthClearValue;
	depthClearValue.depthStencil = { 1.0f, 0 };
	
	VkClearValue clearValues[] =
	{
		colorClearValue,
		depthClearValue
	};
	
	VkExtent2D renderAreaExtent = { SHADOW_MAP_SIZE, SHADOW_MAP_SIZE };
	
	auto renderPassBeginInfo = Framework::Vulkan::RenderPassBeginInfo();
	renderPassBeginInfo.renderPass        = m_shadowMapRenderPass;
	renderPassBeginInfo.renderArea.extent = renderAreaExtent;
	renderPassBeginInfo.clearValueCount   = countof(clearValues);
	renderPassBeginInfo.pClearValues      = clearValues;
	renderPassBeginInfo.framebuffer       = m_shadowMapFramebuffer;
	
	VIEWPORT_PARAMS viewportParams;
	viewportParams.viewMatrix = camera->GetViewMatrix();
	viewportParams.projMatrix = camera->GetProjectionMatrix();
	
	m_device->vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
	
	for(const auto& mesh : renderQueue)
	{
		auto pipeline = m_effect.GetPipelineForMesh(mesh, m_shadowMapRenderPass);
		
		m_device->vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
		
		{
			VkViewport viewport = {};
			viewport.width    = renderAreaExtent.width;
			viewport.height   = renderAreaExtent.height;
			viewport.maxDepth = 1.0f;
			m_device->vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
		
			VkRect2D scissor = {};
			scissor.extent  = renderAreaExtent;
			m_device->vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
		}
		
		auto specVertexBuffer = static_cast<CVulkanVertexBuffer*>(mesh->GetVertexBuffer().get());
		auto vertexBuffer = specVertexBuffer->GetVertexBuffer();
		auto indexBuffer = specVertexBuffer->GetIndexBuffer();
		
		VkDeviceSize offset = 0;
		m_device->vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertexBuffer, &offset);
		m_device->vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT16);
		
		m_effect.UpdateConstants(viewportParams, mesh->GetMaterial().get(), mesh->GetWorldTransformation());
		m_effect.PrepareDraw(commandBuffer);
		
		auto primitiveCount = mesh->GetPrimitiveCount();
		uint32 indexCount = 0;
		switch(mesh->GetPrimitiveType())
		{
		case PRIMITIVE_TRIANGLE_LIST:
			indexCount = primitiveCount * 3;
			break;
		case PRIMITIVE_TRIANGLE_STRIP:
			indexCount = primitiveCount + 2;
			break;
		}
		
		m_device->vkCmdDrawIndexed(commandBuffer, indexCount, 1, 0, 0, 0);
	}
	
	m_device->vkCmdEndRenderPass(commandBuffer);
	
	{
		auto imageMemoryBarrier = Framework::Vulkan::ImageMemoryBarrier();
		imageMemoryBarrier.image               = m_shadowMapImage;
		imageMemoryBarrier.oldLayout           = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		imageMemoryBarrier.newLayout           = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageMemoryBarrier.srcAccessMask       = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		imageMemoryBarrier.dstAccessMask       = VK_ACCESS_SHADER_READ_BIT;
		imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imageMemoryBarrier.subresourceRange    = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
		
		m_device->vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT, VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT,
			0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);
	}
}
