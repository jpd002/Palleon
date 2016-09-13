#pragma once

#include "palleon/vulkan/VulkanEffect.h"
#include "palleon/vulkan/VulkanUberEffectGenerator.h"

namespace Palleon
{
	class CVulkanUberEffect : public CVulkanEffect
	{
	public:
		CVulkanUberEffect(Framework::Vulkan::CDevice&, const CVulkanUberEffectGenerator::EFFECTCAPS&);
		
		void UpdateConstants(const VIEWPORT_PARAMS&, CMaterial*, const CMatrix4&) override;
		void PrepareDraw(VkCommandBuffer) override;
		
	protected:
		struct DefaultPushConstants
		{
			CColor meshColor;
			CMatrix4 viewProjMatrix;
			CMatrix4 worldMatrix;
		};
		
		void    CreatePipelineLayout();
		void    CreateShaderModules(const CVulkanUberEffectGenerator::EFFECTCAPS&);
		
		DefaultPushConstants    m_pushConstants;
	};
}
