#include "palleon/vulkan/VulkanUberEffectProvider.h"
#include "palleon/vulkan/VulkanUberEffectGenerator.h"

using namespace Palleon;

CVulkanUberEffectProvider::CVulkanUberEffectProvider(Framework::Vulkan::CDevice& device)
: m_device(&device)
{
	
}

EffectPtr CVulkanUberEffectProvider::GetEffectForRenderable(CMesh* mesh, bool hasShadowMap)
{
	CVulkanUberEffectGenerator::EFFECTCAPS effectCaps;
	memset(&effectCaps, 0, sizeof(effectCaps));
	
	if(!m_tempEffect)
	{
		m_tempEffect = std::make_shared<CVulkanEffect>(*m_device);
	}
	
	return m_tempEffect;
}
