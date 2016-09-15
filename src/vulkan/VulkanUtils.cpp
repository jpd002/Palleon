#include "palleon/vulkan/VulkanUtils.h"

uint32 Palleon::GetMemoryTypeIndex(const VkPhysicalDeviceMemoryProperties& memoryProperties, uint32 deviceRequirements, uint32 hostRequirements)
{
	for(uint32 i = 0; i < VK_MAX_MEMORY_TYPES; i++)
	{
		if(deviceRequirements & (1 << i))
		{
			const auto& memoryType = memoryProperties.memoryTypes[i];
			if((memoryType.propertyFlags & hostRequirements) == hostRequirements)
			{
				return i;
			}
		}
	}
	return VULKAN_MEMORY_TYPE_INVALID;
}
