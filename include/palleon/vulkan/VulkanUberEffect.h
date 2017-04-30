#pragma once

#include <vector>
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
		typedef std::vector<uint8> ConstantBuffer;
		
		void    CreatePipelineLayout(const CVulkanUberEffectGenerator::EFFECTCAPS&);
		void    CreateShaderModules(const CVulkanUberEffectGenerator::EFFECTCAPS&);
		
		uint32    m_meshColorOffset = -1;
		uint32    m_viewProjMatrixOffset = -1;
		uint32    m_worldMatrixOffset = -1;
		uint32    m_shadowViewProjMatrixOffset = -1;
		
		ConstantBuffer m_vertexConstantBuffer;
	};
}
