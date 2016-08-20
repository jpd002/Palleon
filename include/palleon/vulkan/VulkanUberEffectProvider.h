#pragma once

#include "palleon/graphics/EffectProvider.h"
#include "palleon/vulkan/VulkanEffect.h"

namespace Palleon
{
	class CVulkanUberEffectProvider : public CEffectProvider
	{
	public:
		CVulkanUberEffectProvider(Framework::Vulkan::CDevice&);
		             
		EffectPtr    GetEffectForRenderable(CMesh*, bool) override;
		
	private:
		Framework::Vulkan::CDevice* m_device = nullptr;
		
		EffectPtr    m_tempEffect;
	};
}
