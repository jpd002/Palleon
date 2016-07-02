#pragma once

#include "Types.h"
#include "vulkan/VulkanDef.h"

namespace Palleon
{
	uint32 GetMemoryTypeIndex(const VkPhysicalDeviceMemoryProperties&, uint32, uint32);
}
