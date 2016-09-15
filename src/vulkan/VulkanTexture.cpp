#include <cstring>
#include "palleon/vulkan/VulkanTexture.h"
#include "palleon/vulkan/VulkanUtils.h"
#include "vulkan/StructDefs.h"

using namespace Palleon;

CVulkanTexture::CVulkanTexture(Framework::Vulkan::CDevice& device, const VkPhysicalDeviceMemoryProperties& memoryProperties, VkQueue queue, Framework::Vulkan::CCommandBufferPool& commandBufferPool,
	TEXTURE_FORMAT format, uint32 width, uint32 height)
: m_device(&device)
, m_memoryProperties(&memoryProperties)
, m_queue(queue)
, m_commandBufferPool(&commandBufferPool)
{
	assert(format == TEXTURE_FORMAT_RGBA8888);
	
	m_format = format;
	m_width  = width;
	m_height = height;
	m_isCube = false;
	
	VkResult result = VK_SUCCESS;
	
	{
		auto imageCreateInfo = Framework::Vulkan::ImageCreateInfo();
		imageCreateInfo.imageType     = VK_IMAGE_TYPE_2D;
		imageCreateInfo.format        = VK_FORMAT_R8G8B8A8_UNORM;
		imageCreateInfo.extent.width  = width;
		imageCreateInfo.extent.height = height;
		imageCreateInfo.extent.depth  = 1;
		imageCreateInfo.mipLevels     = 1;
		imageCreateInfo.arrayLayers   = 1;
		imageCreateInfo.samples       = VK_SAMPLE_COUNT_1_BIT;
		imageCreateInfo.sharingMode   = VK_SHARING_MODE_EXCLUSIVE;
		imageCreateInfo.tiling        = VK_IMAGE_TILING_OPTIMAL;
		imageCreateInfo.usage         = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

		result = m_device->vkCreateImage(*m_device, &imageCreateInfo, nullptr, &m_imageHandle);
		CHECKVULKANERROR(result);
	}

	{
		VkMemoryRequirements memoryRequirements = {};
		m_device->vkGetImageMemoryRequirements(*m_device, m_imageHandle, &memoryRequirements);

		auto memoryAllocateInfo = Framework::Vulkan::MemoryAllocateInfo();
		memoryAllocateInfo.allocationSize  = memoryRequirements.size;
		memoryAllocateInfo.memoryTypeIndex = GetMemoryTypeIndex(*m_memoryProperties, memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		result = m_device->vkAllocateMemory(*m_device, &memoryAllocateInfo, nullptr, &m_imageMemoryHandle);
		CHECKVULKANERROR(result);
	}
	
	m_device->vkBindImageMemory(*m_device, m_imageHandle, m_imageMemoryHandle, 0);
	
	{
		auto imageViewCreateInfo = Framework::Vulkan::ImageViewCreateInfo();
		imageViewCreateInfo.image    = m_imageHandle;
		imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		imageViewCreateInfo.format   = VK_FORMAT_R8G8B8A8_UNORM;
		imageViewCreateInfo.components = 
		{
			VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, 
			VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A 
		};
		imageViewCreateInfo.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
		
		result = m_device->vkCreateImageView(*m_device, &imageViewCreateInfo, nullptr, &m_imageViewHandle);
		CHECKVULKANERROR(result);
	}
}

CVulkanTexture::~CVulkanTexture()
{
	if(m_imageViewHandle != VK_NULL_HANDLE)
	{
		m_device->vkDestroyImageView(*m_device, m_imageViewHandle, nullptr);
		m_imageViewHandle = VK_NULL_HANDLE;
	}
	
	if(m_imageMemoryHandle != VK_NULL_HANDLE)
	{
		m_device->vkFreeMemory(*m_device, m_imageMemoryHandle, nullptr);
		m_imageMemoryHandle = VK_NULL_HANDLE;
	}
	
	if(m_imageHandle != VK_NULL_HANDLE)
	{
		m_device->vkDestroyImage(*m_device, m_imageHandle, nullptr);
		m_imageHandle = VK_NULL_HANDLE;
	}
}

void* CVulkanTexture::GetHandle() const
{
	return m_imageViewHandle;
}

void CVulkanTexture::Update(uint32 mipLevel, const void* data)
{
	assert(m_isCube == false);
	
	VkResult result = VK_SUCCESS;
	
	VkBuffer stagingBufferHandle = VK_NULL_HANDLE;
	VkDeviceMemory stagingBufferMemoryHandle = VK_NULL_HANDLE;
	
	//TODO: Get proper value for that
	uint32 dataSize = m_width * m_height * 4;
	
	//Create staging buffer for our texture data
	{
		auto bufferCreateInfo = Framework::Vulkan::BufferCreateInfo();
		bufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		bufferCreateInfo.size  = dataSize;
		
		result = m_device->vkCreateBuffer(*m_device, &bufferCreateInfo, nullptr, &stagingBufferHandle);
		CHECKVULKANERROR(result);
	}
	
	//Create staging buffer memory
	{
		VkMemoryRequirements memoryRequirements = {};
		m_device->vkGetBufferMemoryRequirements(*m_device, stagingBufferHandle, &memoryRequirements);
		
		auto memoryAllocateInfo = Framework::Vulkan::MemoryAllocateInfo();
		memoryAllocateInfo.allocationSize  = memoryRequirements.size;
		memoryAllocateInfo.memoryTypeIndex = GetMemoryTypeIndex(*m_memoryProperties, memoryRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		assert(memoryAllocateInfo.memoryTypeIndex != VULKAN_MEMORY_TYPE_INVALID);
		
		result = m_device->vkAllocateMemory(*m_device, &memoryAllocateInfo, nullptr, &stagingBufferMemoryHandle);
		CHECKVULKANERROR(result);
	}
	
	m_device->vkBindBufferMemory(*m_device, stagingBufferHandle, stagingBufferMemoryHandle, 0);
	
	//Copy image data in buffer
	{
		void* memoryPtr = nullptr;
		result = m_device->vkMapMemory(*m_device, stagingBufferMemoryHandle, 0, dataSize, 0, &memoryPtr);
		CHECKVULKANERROR(result);
		
		memcpy(memoryPtr, data, dataSize);
		
		m_device->vkUnmapMemory(*m_device, stagingBufferMemoryHandle);
	}
	
	auto commandBuffer = m_commandBufferPool->AllocateBuffer();
	
	//Start command buffer
	{
		auto commandBufferBeginInfo = Framework::Vulkan::CommandBufferBeginInfo();
		commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		
		result = m_device->vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo);
		CHECKVULKANERROR(result);
	}
	
	//Transition image from whatever state to TRANSFER_DST_OPTIMAL
	{
		auto imageMemoryBarrier = Framework::Vulkan::ImageMemoryBarrier();
		imageMemoryBarrier.image               = m_imageHandle;
		imageMemoryBarrier.oldLayout           = VK_IMAGE_LAYOUT_UNDEFINED;
		imageMemoryBarrier.newLayout           = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		imageMemoryBarrier.srcAccessMask       = 0;
		imageMemoryBarrier.dstAccessMask       = VK_ACCESS_TRANSFER_WRITE_BIT;
		imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imageMemoryBarrier.subresourceRange    = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
		
		m_device->vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
			0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);
	}
	
	//CopyBufferToImage
	{
		VkBufferImageCopy bufferImageCopy = {};
		bufferImageCopy.bufferRowLength    = m_width;
		bufferImageCopy.imageSubresource   = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 };
		bufferImageCopy.imageExtent.width  = m_width;
		bufferImageCopy.imageExtent.height = m_height;
		bufferImageCopy.imageExtent.depth  = 1;
		
		m_device->vkCmdCopyBufferToImage(commandBuffer, stagingBufferHandle, m_imageHandle, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1, &bufferImageCopy);
	}
	
	//Transition image from TRANSFER_DST_OPTIMAL to SHADER_READ_ONLY_OPTIMAL
	{
		auto imageMemoryBarrier = Framework::Vulkan::ImageMemoryBarrier();
		imageMemoryBarrier.image               = m_imageHandle;
		imageMemoryBarrier.oldLayout           = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		imageMemoryBarrier.newLayout           = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imageMemoryBarrier.srcAccessMask       = VK_ACCESS_TRANSFER_WRITE_BIT;
		imageMemoryBarrier.dstAccessMask       = VK_ACCESS_SHADER_READ_BIT;
		imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imageMemoryBarrier.subresourceRange    = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
		
		m_device->vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
			0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);
	}
	
	//Finish command buffer
	result = m_device->vkEndCommandBuffer(commandBuffer);
	CHECKVULKANERROR(result);
	
	//Submit command buffer
	{
		auto submitInfo = Framework::Vulkan::SubmitInfo();
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers    = &commandBuffer;
		
		result = m_device->vkQueueSubmit(m_queue, 1, &submitInfo, VK_NULL_HANDLE);
		CHECKVULKANERROR(result);
	}
	
	//Wait for queue ops to complete
	result = m_device->vkQueueWaitIdle(m_queue);
	CHECKVULKANERROR(result);
	
	//Destroy staging buffer and memory
	m_device->vkFreeMemory(*m_device, stagingBufferMemoryHandle, nullptr);
	m_device->vkDestroyBuffer(*m_device, stagingBufferHandle, nullptr);
}

void CVulkanTexture::UpdateCubeFace(TEXTURE_CUBE_FACE, const void*)
{
	
}
