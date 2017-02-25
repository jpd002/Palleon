#include "palleon/vulkan/VulkanShadowMapRenderer.h"
#include "palleon/vulkan/VulkanUtils.h"
#include "vulkan/StructDefs.h"
#include "countof.h"

#define SHADOW_MAP_SIZE 2048
#define SHADOW_MAP_FORMAT VK_FORMAT_R32_SFLOAT

using namespace Palleon;

CVulkanShadowMapRenderer::CVulkanShadowMapRenderer(Framework::Vulkan::CDevice& device, const VkPhysicalDeviceMemoryProperties& memoryProperties)
: m_device(&device)
{
	m_shadowMapRenderPass = CreateRenderPass(SHADOW_MAP_FORMAT, VK_FORMAT_UNDEFINED, false);
	CreateMap(memoryProperties);
}

CVulkanShadowMapRenderer::~CVulkanShadowMapRenderer()
{
	m_device->vkDestroyImageView(*m_device, m_shadowMapImageView, nullptr);
	m_device->vkFreeMemory(*m_device, m_shadowMapMemory, nullptr);
	m_device->vkDestroyImage(*m_device, m_shadowMapImage, nullptr);
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
		//depthAttachment
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
//	subpass.pDepthStencilAttachment = &depthRef;
	 
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
		imageCreateInfo.usage         = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
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
	
	{
		VkImageView attachments[] =
		{
			m_shadowMapImageView
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
}
