#include "palleon/vulkan/VulkanGraphicDevice.h"
#include "palleon/vulkan/VulkanVertexBuffer.h"
#include "palleon/vulkan/VulkanTexture.h"
#include "palleon/graphics/Mesh.h"
#include "palleon/Log.h"
#include "vulkan/StructDefs.h"
#include "vulkan/ShaderModule.h"

struct DefaultPushConstants
{
	CMatrix4 viewProjMatrix;
	CMatrix4 worldMatrix;
};

using namespace Palleon;

VKAPI_ATTR VkBool32 VKAPI_CALL DebugReportCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object, size_t location,
	int32_t messageCode, const char* pLayerPrefix, const char* pMessage, void* pUserData)
{
	CLog::GetInstance().Print("%s: %s", pLayerPrefix, pMessage);
	return VK_FALSE;
}

CVulkanGraphicDevice::CVulkanGraphicDevice(const CVector2&, float)
{
	CreateInstance();
	CreateDebugReportCallback();
}

CVulkanGraphicDevice::~CVulkanGraphicDevice()
{
	m_commandBufferPool.Reset();
	for(auto swapChainImageView : m_swapChainImageViews)
	{
		m_device.vkDestroyImageView(m_device, swapChainImageView, nullptr);
	}
	m_device.vkDestroySemaphore(m_device, m_renderCompleteSemaphore, nullptr);
	m_device.vkDestroySemaphore(m_device, m_imageAcquireSemaphore, nullptr);
	m_device.Reset();
	if(m_debugReportCallback != VK_NULL_HANDLE)
	{
		m_vkInstance.vkDestroyDebugReportCallbackEXT(m_vkInstance, m_debugReportCallback, nullptr);
		m_debugReportCallback = VK_NULL_HANDLE;
	}
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
	
	m_commandBufferPool = Framework::Vulkan::CCommandBufferPool(m_device, renderQueueFamily);
	
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
	
	m_screenSize = CVector2(m_surfaceExtents.width, m_surfaceExtents.height);
	m_scaledScreenSize = m_screenSize;
	
	CreateDefaultPipelineLayout();
}

void CVulkanGraphicDevice::Draw()
{
	m_commandBufferPool.ResetBuffers();
	
	VkResult result = VK_SUCCESS;
	
	uint32_t imageIndex = 0;
	result = m_device.vkAcquireNextImageKHR(m_device, m_swapChain, UINT64_MAX, m_imageAcquireSemaphore, VK_NULL_HANDLE, &imageIndex);
	CHECKVULKANERROR(result);
	
	auto swapChainImage = m_swapChainImages[imageIndex];
	auto framebuffer = m_swapChainFramebuffers[imageIndex];
	
	auto commandBuffer = m_commandBufferPool.AllocateBuffer();
	auto commandBufferBeginInfo = Framework::Vulkan::CommandBufferBeginInfo();
	commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
	result = m_device.vkBeginCommandBuffer(commandBuffer, &commandBufferBeginInfo);
	CHECKVULKANERROR(result);
	
	//Transition image from present to color attachment
	{
		auto imageMemoryBarrier = Framework::Vulkan::ImageMemoryBarrier();
		imageMemoryBarrier.image               = swapChainImage;
		imageMemoryBarrier.oldLayout           = VK_IMAGE_LAYOUT_UNDEFINED;
		//imageMemoryBarrier.srcAccessMask       = VK_ACCESS_MEMORY_READ_BIT;
		imageMemoryBarrier.newLayout           = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		imageMemoryBarrier.dstAccessMask       = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		imageMemoryBarrier.subresourceRange    = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
		
		m_device.vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
			0, 0, nullptr, 0, nullptr, 1, &imageMemoryBarrier);
	}
	
	int currViewport = 0;
	for(const auto& viewport : m_viewports)
	{
		if(currViewport == 0)
		{
			DrawViewport(commandBuffer, viewport, framebuffer, m_surfaceExtents);
		}
		currViewport++;
	}
	
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
	
	result = m_device.vkQueueWaitIdle(m_queue);
	CHECKVULKANERROR(result);
}

VertexBufferPtr CVulkanGraphicDevice::CreateVertexBuffer(const VERTEX_BUFFER_DESCRIPTOR& descriptor)
{
	return std::make_shared<CVulkanVertexBuffer>(m_device, m_physicalDeviceMemoryProperties, descriptor);
}

TexturePtr CVulkanGraphicDevice::CreateTexture(TEXTURE_FORMAT format, uint32 width, uint32 height, uint32 mipCount)
{
	return std::make_shared<CVulkanTexture>(m_device, m_physicalDeviceMemoryProperties, m_queue, m_commandBufferPool, format, width, height);
}

TexturePtr CVulkanGraphicDevice::CreateCubeTexture(TEXTURE_FORMAT, uint32 size)
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

SharedGraphicContextPtr CVulkanGraphicDevice::CreateSharedContext()
{
	return SharedGraphicContextPtr();
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
#if defined(VK_USE_PLATFORM_ANDROID_KHR)
	enabledExtensions.push_back(VK_KHR_ANDROID_SURFACE_EXTENSION_NAME);
#elif defined(VK_USE_PLATFORM_XCB_KHR)
	enabledExtensions.push_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
#endif
	
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

void CVulkanGraphicDevice::CreateDebugReportCallback()
{
	if(m_vkInstance.vkCreateDebugReportCallbackEXT == nullptr) return;

	auto callbackCreateInfo = Framework::Vulkan::DebugReportCallbackCreateInfoEXT();
	callbackCreateInfo.flags       = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
	callbackCreateInfo.pfnCallback = &DebugReportCallback;

	auto result = m_vkInstance.vkCreateDebugReportCallbackEXT(m_vkInstance, &callbackCreateInfo, nullptr, &m_debugReportCallback);
	CHECKVULKANERROR(result);

	CLog::GetInstance().Print("Created debug report callback.");
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
	assert(!m_commandBufferPool.IsEmpty());
	
	VkResult result = VK_SUCCESS;
	auto commandBuffer = m_commandBufferPool.AllocateBuffer();
	
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

void CVulkanGraphicDevice::DrawViewport(VkCommandBuffer commandBuffer, CViewport* viewport, VkFramebuffer framebuffer, VkExtent2D renderAreaExtent)
{
	RenderQueue renderQueue;

	auto camera = viewport->GetCamera();
	assert(camera);
	auto cameraFrustum = camera->GetFrustum();
	
	const auto& sceneRoot = viewport->GetSceneRoot();
	sceneRoot->TraverseNodes(
		[&] (const SceneNodePtr& node)
		{
			if(!node->GetVisible()) return false;
			
			if(auto mesh = std::dynamic_pointer_cast<CMesh>(node))
			{
				if(mesh->GetPrimitiveCount() != 0)
				{
					bool render = true;
					auto boundingSphere = mesh->GetBoundingSphere();
					if(boundingSphere.radius != 0)
					{
						auto worldBoundingSphere = mesh->GetWorldBoundingSphere();
						render = cameraFrustum.Intersects(worldBoundingSphere);
					}
					if(render)
					{
						renderQueue.push_back(mesh.get());
					}
				}
			}
			
			return true;
		}
	);
	
	VkClearValue clearValue;
	clearValue.color = { { 0.0f, 0.0f, 0.0f, 1.0f } };
	
	auto renderPassBeginInfo = Framework::Vulkan::RenderPassBeginInfo();
	renderPassBeginInfo.renderPass               = m_renderPass;
	renderPassBeginInfo.renderArea.extent        = renderAreaExtent;
	renderPassBeginInfo.clearValueCount          = 1;
	renderPassBeginInfo.pClearValues             = &clearValue;
	renderPassBeginInfo.framebuffer              = framebuffer;
	
	m_device.vkCmdBeginRenderPass(commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
	
	for(const auto& mesh : renderQueue)
	{
		DefaultPushConstants pushConstants;
		pushConstants.viewProjMatrix = camera->GetViewMatrix() * camera->GetProjectionMatrix();
		pushConstants.worldMatrix = mesh->GetWorldTransformation();
		
		auto pipeline = GetPipelineForMesh(mesh);
		
		m_device.vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
		
		{
			VkViewport viewport = {};
			viewport.width    = renderAreaExtent.width;
			viewport.height   = renderAreaExtent.height;
			viewport.maxDepth = 1.0f;
			m_device.vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
		
			VkRect2D scissor = {};
			scissor.extent  = renderAreaExtent;
			m_device.vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
		}
		
		auto specVertexBuffer = static_cast<CVulkanVertexBuffer*>(mesh->GetVertexBuffer().get());
		auto vertexBuffer = specVertexBuffer->GetVertexBuffer();
		auto indexBuffer = specVertexBuffer->GetIndexBuffer();
		
		VkDeviceSize offset = 0;
		m_device.vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertexBuffer, &offset);
		m_device.vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT16);
		
		m_device.vkCmdPushConstants(commandBuffer, m_defaultPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(DefaultPushConstants), &pushConstants);
		
		auto primitiveCount = mesh->GetPrimitiveCount();
		assert(mesh->GetPrimitiveType() == PRIMITIVE_TRIANGLE_LIST);
		m_device.vkCmdDrawIndexed(commandBuffer, primitiveCount * 3, 1, 0, 0, 0);
	}
	
	m_device.vkCmdEndRenderPass(commandBuffer);
}

void CVulkanGraphicDevice::CreateDefaultPipelineLayout()
{
	VkPushConstantRange pushConstantInfo = {};
	pushConstantInfo.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	pushConstantInfo.offset     = 0;
	pushConstantInfo.size       = sizeof(DefaultPushConstants);
	
	auto pipelineLayoutCreateInfo = Framework::Vulkan::PipelineLayoutCreateInfo();
	pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
	pipelineLayoutCreateInfo.pPushConstantRanges    = &pushConstantInfo;
	
	auto result = m_device.vkCreatePipelineLayout(m_device, &pipelineLayoutCreateInfo, nullptr, &m_defaultPipelineLayout);
	CHECKVULKANERROR(result);
}

VkPipeline CVulkanGraphicDevice::GetPipelineForMesh(CMesh* mesh)
{
	assert(m_defaultPipelineLayout != VK_NULL_HANDLE);
	
	VkPipeline pipeline = VK_NULL_HANDLE;
	
	assert(mesh->GetPrimitiveType() == PRIMITIVE_TRIANGLE_LIST);
	//assert(mesh->GetPrimitiveType() == PRIMITIVE_TRIANGLE_STRIP);
	
	const auto& vertexBufferDescriptor = mesh->GetVertexBuffer()->GetDescriptor();
	auto pipelineIterator = m_pipelines.find(vertexBufferDescriptor.vertexItems);
	if(pipelineIterator != std::end(m_pipelines))
	{
		return pipelineIterator->second;
	}

	auto inputAssemblyInfo = Framework::Vulkan::PipelineInputAssemblyStateCreateInfo();
	inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	//inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;

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
	
	auto vertexShaderStream = CResourceManager::GetInstance().MakeResourceStream("triangle.vert.spv");
	Framework::Vulkan::CShaderModule vertexShaderModule(m_device, *vertexShaderStream);
	
	auto pixelShaderStream = CResourceManager::GetInstance().MakeResourceStream("triangle.frag.spv");
	Framework::Vulkan::CShaderModule pixelShaderModule(m_device, *pixelShaderStream);
	
	// Load our SPIR-V shaders.
	VkPipelineShaderStageCreateInfo shaderStages[2] =
	{
		{ VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO },
		{ VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO },
	};
	
	//We have two pipeline stages, vertex and fragment.
	shaderStages[0].stage  = VK_SHADER_STAGE_VERTEX_BIT;
	shaderStages[0].module = vertexShaderModule;
	shaderStages[0].pName  = "main";
	shaderStages[1].stage  = VK_SHADER_STAGE_FRAGMENT_BIT;
	shaderStages[1].module = pixelShaderModule;
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
	pipelineCreateInfo.renderPass          = m_renderPass;
	pipelineCreateInfo.layout              = m_defaultPipelineLayout;
	
	auto result = m_device.vkCreateGraphicsPipelines(m_device, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &pipeline);
	CHECKVULKANERROR(result);
	
	m_pipelines.insert(std::make_pair(vertexBufferDescriptor.vertexItems, pipeline));
	
	return pipeline;
}
