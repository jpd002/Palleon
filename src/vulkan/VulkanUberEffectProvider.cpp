#include "palleon/vulkan/VulkanUberEffectProvider.h"

using namespace Palleon;

CVulkanUberEffectProvider::CVulkanUberEffectProvider(Framework::Vulkan::CDevice& device)
: m_device(&device)
{
	
}

EffectPtr CVulkanUberEffectProvider::GetEffectForRenderable(CMesh* mesh, bool hasShadowMap)
{
	CVulkanUberEffectGenerator::EFFECTCAPS effectCaps;
	memset(&effectCaps, 0, sizeof(effectCaps));
	
	uint32 effectKey = *reinterpret_cast<uint32*>(&effectCaps);
	auto effectIterator = m_effects.find(effectKey);
	if(effectIterator == std::end(m_effects))
	{
		auto effect = std::make_shared<CVulkanUberEffect>(*m_device, effectCaps);
		m_effects.insert(std::make_pair(effectKey, effect));
		effectIterator = m_effects.find(effectKey);
	}
	
	return effectIterator->second;
}
