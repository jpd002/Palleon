#include "palleon/vulkan/VulkanGraphicDevice.h"
#include "palleon/vulkan/VulkanVertexBuffer.h"
#include "palleon/vulkan/VulkanTexture.h"
#include "palleon/Log.h"
#include "vulkan/StructDefs.h"

using namespace Palleon;

CVulkanGraphicDevice::CVulkanGraphicDevice(const CVector2&, float)
{
	CreateInstance();
}

CVulkanGraphicDevice::~CVulkanGraphicDevice()
{
	
}

void CVulkanGraphicDevice::Initialize()
{
	auto physicalDevices = GetPhysicalDevices();
	assert(physicalDevices.size() > 0);
	auto physicalDevice = physicalDevices[0];
	
	auto renderQueueFamilies = GetRenderQueueFamilies(physicalDevice);
	assert(renderQueueFamilies.size() > 0);
	auto renderQueueFamily = renderQueueFamilies[0];
	
	auto surfaceFormats = GetDeviceSurfaceFormats(physicalDevice);
	assert(surfaceFormats.size() > 0);
	auto surfaceFormat = surfaceFormats[0];
	
	m_vkInstance.vkGetPhysicalDeviceMemoryProperties(physicalDevice, &m_physicalDeviceMemoryProperties);
	
	{
		VkSurfaceCapabilitiesKHR surfaceCaps = {};
		auto result = m_vkInstance.vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, m_surface, &surfaceCaps);
		CHECKVULKANERROR(result);
		CLog::GetInstance().Print("Surface Current Extents: %d, %d", 
			surfaceCaps.currentExtent.width, surfaceCaps.currentExtent.height);
		m_surfaceExtents = surfaceCaps.currentExtent;
	}
	
	CreateDevice(physicalDevice);
	m_device.vkGetDeviceQueue(m_device, renderQueueFamily, 0, &m_queue);
	
	m_commandPool = CreateCommandPool(renderQueueFamilies[0]);
	
	m_renderPass = CreateRenderPass(surfaceFormat.format);
	
	CreateSwapChain(surfaceFormat, m_surfaceExtents);
	PrepareSwapChainImages();
	CreateSwapChainImageViews(surfaceFormat.format);
	CreateSwapChainFramebuffers(m_renderPass, m_surfaceExtents);
	
	//Create the semaphore that will be used to prevent submit from rendering before getting the image
	{
		auto semaphoreCreateInfo = Framework::Vulkan::SemaphoreCreateInfo();
		auto result = m_device.vkCreateSemaphore(m_device, &semaphoreCreateInfo, nullptr, &m_imageAcquireSemaphore);
		CHECKVULKANERROR(result);
	}
	
	{
		auto semaphoreCreateInfo = Framework::Vulkan::SemaphoreCreateInfo();
		auto result = m_device.vkCreateSemaphore(m_device, &semaphoreCreateInfo, nullptr, &m_renderCompleteSemaphore);
		CHECKVULKANERROR(result);
	}
}

void CVulkanGraphicDevice::Draw()
{
	auto commandBuffers = AllocateCommandBuffers(m_commandPool, 1);
	auto commandBuffer = commandBuffers[0];
	
	VkResult result = VK_SUCCESS;
	
	uint32_t imageIndex = 0;
	result = m_device.vkAcquireNextImageKHR(m_device, m_swapChain, UINT64_MAX, m_imageAcquireSemaphore, VK_NULL_HANDLE, &imageIndex);
	CHECKVULKANERROR(result);
	
	auto swapChainImage = m_swapChainImages[imageIndex];
	auto framebuffer = m_swapChainFramebuffers[imageIndex];
	
	BuildClearCommandList(commandBuffer, swapChainImage, m_surfaceExtents, m_renderPass, framebuffer);
	
	{
		VkPipelineStageFlags pipelineStageFlags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		auto submitInfo = Framework::Vulkan::SubmitInfo();
		submitInfo.waitSemaphoreCount   = 1;
		submitInfo.pWaitSemaphores      = &m_imageAcquireSemaphore;
		submitInfo.pWaitDstStageMask    = &pipelineStageFlags;
		submitInfo.commandBufferCount   = 1;
		submitInfo.pCommandBuffers      = &commandBuffer;
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores    = &m_renderCompleteSemaphore;
		result = m_device.vkQueueSubmit(m_queue, 1, &submitInfo, VK_NULL_HANDLE);
		CHECKVULKANERROR(result);
	}
	
	{
		auto presentInfo = Framework::Vulkan::PresentInfoKHR();
		presentInfo.swapchainCount     = 1;
		presentInfo.pSwapchains        = &m_swapChain;
		presentInfo.pImageIndices      = &imageIndex;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores    = &m_renderCompleteSemaphore;
		result = m_device.vkQueuePresentKHR(m_queue, &presentInfo);
		CHECKVULKANERROR(result);
	}
	
	//result = device.vkQueueWaitIdle(queue);
	//CheckResult(result);
	
	FreeCommandBuffer(m_commandPool, commandBuffer);
}

VertexBufferPtr CVulkanGraphicDevice::CreateVertexBuffer(const VERTEX_BUFFER_DESCRIPTOR& descriptor)
{
	return std::make_shared<CVulkanVertexBuffer>(m_device, m_physicalDeviceMemoryProperties, descriptor);
}

TexturePtr CVulkanGraphicDevice::CreateTexture(TEXTURE_FORMAT, uint32, uint32, uint32)
{
	return std::make_shared<CVulkanTexture>();
}

TexturePtr CVulkanGraphicDevice::CreateCubeTexture(TEXTURE_FORMAT, uint32)
{
	return std::make_shared<CVulkanTexture>();
}

RenderTargetPtr CVulkanGraphicDevice::CreateRenderTarget(TEXTURE_FORMAT, uint32, uint32)
{
	return RenderTargetPtr();
}

CubeRenderTargetPtr CVulkanGraphicDevice::CreateCubeRenderTarget(TEXTURE_FORMAT, uint32)
{
	return CubeRenderTargetPtr();
}

void CVulkanGraphicDevice::CreateInstance()
{
	assert(m_vkInstance.IsEmpty());
	
	auto appInfo = Framework::Vulkan::ApplicationInfo();
	appInfo.pApplicationName = "Palleon";
	appInfo.pEngineName      = "Palleon";
	appInfo.apiVersion       = VK_MAKE_VERSION(1, 0, 2);		//Required on nVidia Shield TV
	//appInfo.apiVersion       = VK_API_VERSION;
	
	std::vector<const char*> enabledExtensions;
	//enabledExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
	enabledExtensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
	enabledExtensions.push_back(VK_KHR_ANDROID_SURFACE_EXTENSION_NAME);
	
	std::vector<const char*> enabledLayers;
	//enabledLayers.push_back("VK_LAYER_GOOGLE_threading");
	//enabledLayers.push_back("VK_LAYER_LUNARG_param_checker");
	//enabledLayers.push_back("VK_LAYER_LUNARG_object_tracker");
	
	auto instanceCreateInfo = Framework::Vulkan::InstanceCreateInfo();
	instanceCreateInfo.pApplicationInfo        = &appInfo;
	instanceCreateInfo.enabledExtensionCount   = enabledExtensions.size();
	instanceCreateInfo.ppEnabledExtensionNames = enabledExtensions.data();
	instanceCreateInfo.enabledLayerCount       = enabledLayers.size();
	instanceCreateInfo.ppEnabledLayerNames     = enabledLayers.data();
	m_vkInstance = Framework::Vulkan::CInstance(instanceCreateInfo);
	
	CLog::GetInstance().Print("Created Vulkan instance.");
}

void CVulkanGraphicDevice::CreateDevice(VkPhysicalDevice physicalDevice)
{
	assert(m_device.IsEmpty());
	
	float queuePriorities[] = { 1.0f };
	
	auto deviceQueueCreateInfo = Framework::Vulkan::DeviceQueueCreateInfo();
	deviceQueueCreateInfo.flags            = 0;
	deviceQueueCreateInfo.queueFamilyIndex = 0;
	deviceQueueCreateInfo.queueCount       = 1;
	deviceQueueCreateInfo.pQueuePriorities = queuePriorities;
	
	std::vector<const char*> enabledExtensions;
	//enabledExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
	
	auto deviceCreateInfo = Framework::Vulkan::DeviceCreateInfo();
	deviceCreateInfo.flags                   = 0;
	deviceCreateInfo.enabledLayerCount       = 0;
	deviceCreateInfo.ppEnabledLayerNames     = nullptr;
	deviceCreateInfo.enabledExtensionCount   = enabledExtensions.size();
	deviceCreateInfo.ppEnabledExtensionNames = enabledExtensions.data();
	deviceCreateInfo.pEnabledFeatures        = nullptr;
	deviceCreateInfo.queueCreateInfoCount    = 1;
	deviceCreateInfo.pQueueCreateInfos       = &deviceQueueCreateInfo;
	
	m_device = Framework::Vulkan::CDevice(m_vkInstance, physicalDevice, deviceCreateInfo);
	
	CLog::GetInstance().Print("Created device.");
}

VkRenderPass CVulkanGraphicDevice::CreateRenderPass(VkFormat colorFormat)
{
	assert(!m_device.IsEmpty());
	
	auto result = VK_SUCCESS;
	
	VkAttachmentDescription attachment = {};
	attachment.format         = colorFormat;
	attachment.samples        = VK_SAMPLE_COUNT_1_BIT;
	attachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachment.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
	attachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachment.initialLayout  = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	attachment.finalLayout    = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	
	VkAttachmentReference colorRef = {};
	colorRef.attachment = 0;
	colorRef.layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	
	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint    = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments    = &colorRef;

	auto renderPassCreateInfo = Framework::Vulkan::RenderPassCreateInfo();
	renderPassCreateInfo.attachmentCount = 1;
	renderPassCreateInfo.pAttachments    = &attachment;
	renderPassCreateInfo.subpassCount    = 1;
	renderPassCreateInfo.pSubpasses      = &subpass;
	
	VkRenderPass renderPass = VK_NULL_HANDLE;
	result = m_device.vkCreateRenderPass(m_device, &renderPassCreateInfo, nullptr, &renderPass);
	CHECKVULKANERROR(result);
	
	return renderPass;
}

std::vector<VkPhysicalDevice> CVulkanGraphicDevice::GetPhysicalDevices()
{
	auto result = VK_SUCCESS;

	uint32_t physicalDeviceCount = 0;
	result = m_vkInstance.vkEnumeratePhysicalDevices(m_vkInstance, &physicalDeviceCount, nullptr);
	CHECKVULKANERROR(result);
	
	CLog::GetInstance().Print("Found %d physical devices.", physicalDeviceCount);
	
	std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
	result = m_vkInstance.vkEnumeratePhysicalDevices(m_vkInstance, &physicalDeviceCount, physicalDevices.data());
	CHECKVULKANERROR(result);
	
	for(const auto& physicalDevice : physicalDevices)
	{
		CLog::GetInstance().Print("Physical Device Info:");
		
		VkPhysicalDeviceProperties physicalDeviceProperties = {};
		m_vkInstance.vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);
		CLog::GetInstance().Print("Driver Version: %d", physicalDeviceProperties.driverVersion);
		CLog::GetInstance().Print("Device Name:    %s", physicalDeviceProperties.deviceName);
		CLog::GetInstance().Print("Device Type:    %d", physicalDeviceProperties.deviceType);
		CLog::GetInstance().Print("API Version:    %d.%d.%d",
			VK_VERSION_MAJOR(physicalDeviceProperties.apiVersion),
			VK_VERSION_MINOR(physicalDeviceProperties.apiVersion),
			VK_VERSION_PATCH(physicalDeviceProperties.apiVersion));
	}
	
	return physicalDevices;
}

std::vector<uint32_t> CVulkanGraphicDevice::GetRenderQueueFamilies(VkPhysicalDevice physicalDevice)
{
	assert(m_surface != VK_NULL_HANDLE);
	
	auto result = VK_SUCCESS;
	std::vector<uint32_t> renderQueueFamilies;
	
	uint32_t queueFamilyCount = 0;
	m_vkInstance.vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
	
	CLog::GetInstance().Print("Found %d queue families.", queueFamilyCount);
	
	std::vector<VkQueueFamilyProperties> queueFamilyPropertiesArray(queueFamilyCount);
	m_vkInstance.vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilyPropertiesArray.data());
	
	for(uint32_t queueFamilyIndex = 0; queueFamilyIndex < queueFamilyCount; queueFamilyIndex++)
	{
		bool graphicsSupported = false;
		
		CLog::GetInstance().Print("Queue Family Info:");

		const auto& queueFamilyProperties = queueFamilyPropertiesArray[queueFamilyIndex];
		CLog::GetInstance().Print("Queue Count:    %d", queueFamilyProperties.queueCount);
		CLog::GetInstance().Print("Operating modes:");
		if(queueFamilyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			graphicsSupported = true;
			CLog::GetInstance().Print("  Graphics");
		}
		if(queueFamilyProperties.queueFlags & VK_QUEUE_COMPUTE_BIT)
		{
			CLog::GetInstance().Print("  Compute");
		}
		if(queueFamilyProperties.queueFlags & VK_QUEUE_TRANSFER_BIT)
		{
			CLog::GetInstance().Print("  Transfer");
		}
		if(queueFamilyProperties.queueFlags & VK_QUEUE_SPARSE_BINDING_BIT)
		{
			CLog::GetInstance().Print("  Sparse Binding");
		}
		
		VkBool32 surfaceSupported = VK_FALSE;
		result = m_vkInstance.vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, queueFamilyIndex, m_surface, &surfaceSupported);
		CHECKVULKANERROR(result);
		
		CLog::GetInstance().Print("Supports surface: %d", surfaceSupported);
		
		if(graphicsSupported && surfaceSupported)
		{
			renderQueueFamilies.push_back(queueFamilyIndex);
		}
	}
	
	return renderQueueFamilies;
}

std::vector<VkSurfaceFormatKHR> CVulkanGraphicDevice::GetDeviceSurfaceFormats(VkPhysicalDevice physicalDevice)
{
	assert(m_surface != VK_NULL_HANDLE);

	auto result = VK_SUCCESS;
	
	uint32_t surfaceFormatCount = 0;
	result = m_vkInstance.vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, m_surface, &surfaceFormatCount, nullptr);
	CHECKVULKANERROR(result);
	
	CLog::GetInstance().Print("Found %d surface formats.", surfaceFormatCount);

	std::vector<VkSurfaceFormatKHR> surfaceFormats(surfaceFormatCount);
	result = m_vkInstance.vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, m_surface, &surfaceFormatCount, surfaceFormats.data());
	CHECKVULKANERROR(result);
	
	for(const auto& surfaceFormat : surfaceFormats)
	{
		CLog::GetInstance().Print("Surface Format Info:");
		
		CLog::GetInstance().Print("Format:      %d", surfaceFormat.format);
		CLog::GetInstance().Print("Color Space: %d", surfaceFormat.colorSpace);
	}
	
	return surfaceFormats;
}

//////////////////////////////////////////////////////////////
//Command Pool Stuff
//////////////////////////////////////////////////////////////

VkCommandPool CVulkanGraphicDevice::CreateCommandPool(uint32_t queueFamilyIndex)
{
	assert(!m_device.IsEmpty());
	
	auto commandPoolCreateInfo = Framework::Vulkan::CommandPoolCreateInfo();
	commandPoolCreateInfo.queueFamilyIndex = queueFamilyIndex;
	commandPoolCreateInfo.flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

	VkCommandPool commandPool = VK_NULL_HANDLE;
	auto result = m_device.vkCreateCommandPool(m_device, &commandPoolCreateInfo, nullptr, &commandPool);
	CHECKVULKANERROR(result);
	
	CLog::GetInstance().Print("Created command pool");
	
	return commandPool;
}

std::vector<VkCommandBuffer> CVulkanGraphicDevice::AllocateCommandBuffers(VkCommandPool commandPool, uint32_t commandBufferCount)
{
	assert(!m_device.IsEmpty());
	
	auto bufferAllocateInfo = Framework::Vulkan::CommandBufferAllocateInfo();
	bufferAllocateInfo.commandPool        = commandPool;
	bufferAllocateInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	bufferAllocateInfo.commandBufferCount = commandBufferCount;
	
	std::vector<VkCommandBuffer> commandBuffers(commandBufferCount);
	auto result = m_device.vkAllocateCommandBuffers(m_device, &bufferAllocateInfo, commandBuffers.data());
	CHECKVULKANERROR(result);
	
	return commandBuffers;
}

void CVulkanGraphicDevice::FreeCommandBuffer(VkCommandPool commandPool, VkCommandBuffer commandBuffer)
{
	assert(!m_device.IsEmpty());
	
	m_device.vkFreeCommandBuffers(m_device, commandPool, 1, &commandBuffer);
}

//////////////////////////////////////////////////////////////
//Swap Chain Stuff
//////////////////////////////////////////////////////////////

void CVulkanGraphicDevice::CreateSwapChain(VkSurfaceFormatKHR surfaceFormat, VkExtent2D imageExtent)
{
	assert(!m_device.IsEmpty());
	assert(m_swapChainImages.empty());
	
	auto result = VK_SUCCESS;
	
	auto swapChainCreateInfo = Framework::Vulkan::SwapchainCreateInfoKHR();
	swapChainCreateInfo.surface               = m_surface;
	swapChainCreateInfo.minImageCount         = 3; //Recommended by nVidia in UsingtheVulkanAPI_20160216.pdf
	swapChainCreateInfo.imageFormat           = surfaceFormat.format;
	swapChainCreateInfo.imageColorSpace       = surfaceFormat.colorSpace;
	swapChainCreateInfo.imageExtent           = imageExtent;
	swapChainCreateInfo.imageUsage            = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	swapChainCreateInfo.preTransform          = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
	swapChainCreateInfo.imageArrayLayers      = 1;
	swapChainCreateInfo.imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE;
	swapChainCreateInfo.queueFamilyIndexCount = 0;
	swapChainCreateInfo.pQueueFamilyIndices   = nullptr;
	swapChainCreateInfo.presentMode           = VK_PRESENT_MODE_FIFO_KHR;
	swapChainCreateInfo.clipped               = VK_TRUE;
	swapChainCreateInfo.compositeAlpha        = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

	result = m_device.vkCreateSwapchainKHR(m_device, &swapChainCreateInfo, nullptr, &m_swapChain);
	CHECKVULKANERROR(result);
	
	uint32_t imageCount = 0;
	result = m_device.vkGetSwapchainImagesKHR(m_device, m_swapChain, &imageCount, nullptr);
	CHECKVULKANERROR(result);
	
	m_swapChainImages.resize(imageCount);
	result = m_device.vkGetSwapchainImagesKHR(m_device, m_swapChain, &imageCount, m_swapChainImages.data());
	CHECKVULKANERROR(result);
}

void CVulkanGraphicDevice::PrepareSwapChainImages()
{
	assert(!m_device.IsEmpty());
	assert(m_commandPool != VK_NULL_HANDLE);
	
	VkResult result = VK_SUCCESS;
	auto commandBuffers = AllocateCommandBuffers(m_commandPool, 1);
	auto commandBuffer = commandBuffers[0];
	
	auto commandBufferBeginInfo = Framework::Vulkan::CommandBufferBeginInfo();
	result = m_device.vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo);
	CHECKVULKANERROR(result);
	
	for(const auto& image : m_swapChainImages)
	{
		auto imageMemoryBarrier = Framework::Vulkan::ImageMemoryBarrier();
		imageMemoryBarrier.image               = image;
		imageMemoryBarrier.oldLayout           = VK_IMAGE_LAYOUT_UNDEFINED;
		imageMemoryBarrier.newLayout           = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imageMemoryBarrier.subresourceRange    = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
		
		m_device.vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
			0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);
	}
	
	result = m_device.vkEndCommandBuffer(commandBuffer);
	CHECKVULKANERROR(result);
	
	auto submitInfo = Framework::Vulkan::SubmitInfo();
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers    = &commandBuffer;
	result = m_device.vkQueueSubmit(m_queue, 1, &submitInfo, VK_NULL_HANDLE);
	CHECKVULKANERROR(result);

	result = m_device.vkQueueWaitIdle(m_queue);
	CHECKVULKANERROR(result);
	
	FreeCommandBuffer(m_commandPool, commandBuffer);
}

void CVulkanGraphicDevice::CreateSwapChainImageViews(VkFormat colorFormat)
{
	assert(!m_device.IsEmpty());
	assert(m_swapChainImageViews.empty());
	
	for(const auto& image : m_swapChainImages)
	{
		auto imageViewCreateInfo = Framework::Vulkan::ImageViewCreateInfo();
		imageViewCreateInfo.format     = colorFormat;
		imageViewCreateInfo.viewType   = VK_IMAGE_VIEW_TYPE_2D;
		imageViewCreateInfo.image      = image;
		imageViewCreateInfo.components = 
		{ 
			VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, 
			VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A 
		};
		imageViewCreateInfo.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
		
		VkImageView imageView = VK_NULL_HANDLE;
		auto result = m_device.vkCreateImageView(m_device, &imageViewCreateInfo, nullptr, &imageView);
		CHECKVULKANERROR(result);
		m_swapChainImageViews.push_back(imageView);
	}
}

void CVulkanGraphicDevice::CreateSwapChainFramebuffers(VkRenderPass renderPass, VkExtent2D size)
{
	assert(!m_device.IsEmpty());
	
	for(const auto& imageView : m_swapChainImageViews)
	{
		auto frameBufferCreateInfo = Framework::Vulkan::FramebufferCreateInfo();
		frameBufferCreateInfo.renderPass      = renderPass;
		frameBufferCreateInfo.attachmentCount = 1;
		frameBufferCreateInfo.pAttachments    = &imageView;
		frameBufferCreateInfo.width           = size.width;
		frameBufferCreateInfo.height          = size.height;
		frameBufferCreateInfo.layers          = 1;
		
		VkFramebuffer framebuffer = VK_NULL_HANDLE;
		auto result = m_device.vkCreateFramebuffer(m_device, &frameBufferCreateInfo, nullptr, &framebuffer);
		CHECKVULKANERROR(result);
		m_swapChainFramebuffers.push_back(framebuffer);
	}
}


//////////////////////////////////////////////////////////////
//Rendering
//////////////////////////////////////////////////////////////

void CVulkanGraphicDevice::BuildClearCommandList(VkCommandBuffer commandBuffer, VkImage swapChainImage, VkExtent2D renderAreaExtent, 
	VkRenderPass renderPass, VkFramebuffer framebuffer)
{
	static float t = 0;
	
	VkResult result = VK_SUCCESS;
	
	auto commandBufferBeginInfo = Framework::Vulkan::CommandBufferBeginInfo();
	commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
	result = m_device.vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo);
	CHECKVULKANERROR(result);
	
	//Transition image from present to color attachment
	{
		auto imageMemoryBarrier = Framework::Vulkan::ImageMemoryBarrier();
		imageMemoryBarrier.image               = swapChainImage;
		imageMemoryBarrier.oldLayout           = VK_IMAGE_LAYOUT_UNDEFINED;
		imageMemoryBarrier.srcAccessMask       = VK_ACCESS_MEMORY_READ_BIT;
		imageMemoryBarrier.newLayout           = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		imageMemoryBarrier.dstAccessMask       = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imageMemoryBarrier.subresourceRange    = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
		
		m_device.vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);
	}
	
	//Execute render pass
	float color = static_cast<float>(fabs(sin(t)));
	//Log_Print("Color: %f", color);
	VkClearColorValue defaultClearColor = { { 0.0f, 0.0f, color, 1.0f } };
	t += 0.01f;
	
#if 1
	VkClearValue clearValue;
	clearValue.color = defaultClearColor;
	
	auto renderPassBeginInfo = Framework::Vulkan::RenderPassBeginInfo();
	renderPassBeginInfo.renderPass               = renderPass;
	renderPassBeginInfo.renderArea.extent        = renderAreaExtent;
	renderPassBeginInfo.clearValueCount          = 1;
	renderPassBeginInfo.pClearValues             = &clearValue;
	renderPassBeginInfo.framebuffer              = framebuffer;
	
	m_device.vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
	
	//VkViewport viewport = {};
	//viewport.width    = renderAreaExtent.width;
	//viewport.height   = renderAreaExtent.height;
	//viewport.maxDepth = 1.0f;
	//m_device.vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
	
	//VkRect2D scissor = {};
	//scissor.extent  = renderAreaExtent;
	//scissor.extent.width = 32;
	//scissor.extent.height = 32;
	//m_device.vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
	
	m_device.vkCmdEndRenderPass(commandBuffer);
#else
	VkImageSubresourceRange imageSubresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
	//m_device.vkCmdClearColorImage(commandBuffer, swapChainImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &defaultClearColor, 1, &imageSubresourceRange);
	m_device.vkCmdClearColorImage(commandBuffer, swapChainImage, VK_IMAGE_LAYOUT_GENERAL, &defaultClearColor, 1, &imageSubresourceRange);
#endif
		
	//Transition image from attachment to present
	{
		auto imageMemoryBarrier = Framework::Vulkan::ImageMemoryBarrier();
		imageMemoryBarrier.image               = swapChainImage;
		imageMemoryBarrier.oldLayout           = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		imageMemoryBarrier.srcAccessMask       = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		imageMemoryBarrier.newLayout           = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		imageMemoryBarrier.dstAccessMask       = VK_ACCESS_MEMORY_READ_BIT;
		imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imageMemoryBarrier.subresourceRange    = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
		
		m_device.vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
			0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);
	}
	
	result = m_device.vkEndCommandBuffer(commandBuffer);
	CHECKVULKANERROR(result);
}
