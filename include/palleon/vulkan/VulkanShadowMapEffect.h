#pragma once

#include "VulkanEffect.h"
#include "nuanceur/Builder.h"

namespace Palleon
{
	class CVulkanShadowMapEffect : public CVulkanEffect
	{
	public:
		CVulkanShadowMapEffect(Framework::Vulkan::CDevice&);
		
		void UpdateConstants(const VIEWPORT_PARAMS&, CMaterial*, const CMatrix4&) override;
		void PrepareDraw(VkCommandBuffer) override;
		
	private:
		struct DefaultPushConstants
		{
			CMatrix4 viewProjMatrix;
			CMatrix4 worldMatrix;
		};
		
		void    CreatePipelineLayout();
		
		Nuanceur::CShaderBuilder BuildVertexShader();
		Nuanceur::CShaderBuilder BuildFragmentShader();
		
		DefaultPushConstants    m_pushConstants;
	};
}
