#pragma once

#include "palleon/graphics/EffectProvider.h"
#include "palleon/vulkan/VulkanUberEffect.h"

namespace Palleon
{
	class CVulkanUberEffectProvider : public CEffectProvider
	{
	public:
		CVulkanUberEffectProvider(Framework::Vulkan::CDevice&);
		
		EffectPtr    GetEffectForRenderable(CMesh*, bool) override;
		
	private:
		typedef std::unordered_map<uint32, VulkanEffectPtr> EffectMap;
		
		Framework::Vulkan::CDevice* m_device = nullptr;
		
		EffectMap    m_effects;
	};
}
