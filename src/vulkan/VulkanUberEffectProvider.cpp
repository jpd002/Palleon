#include "palleon/vulkan/VulkanUberEffectProvider.h"

using namespace Palleon;

CVulkanUberEffectProvider::CVulkanUberEffectProvider(Framework::Vulkan::CDevice& device)
: m_device(&device)
{
	
}

EffectPtr CVulkanUberEffectProvider::GetEffectForRenderable(CMesh* mesh, bool hasShadowMap)
{
	const auto& descriptor = mesh->GetVertexBuffer()->GetDescriptor();
	
	auto material = mesh->GetMaterial();
	assert(material != nullptr);
	
	CVulkanUberEffectGenerator::EFFECTCAPS effectCaps;
	memset(&effectCaps, 0, sizeof(effectCaps));
	
	effectCaps.hasShadowMap = hasShadowMap && material->GetShadowReceiving();
	
	auto texture = material->GetTexture(0);
	if(texture)
	{
		auto textureCoordSource = material->GetTextureCoordSource(0);
		assert(descriptor.HasVertexItem(VERTEX_ITEM_ID_UV0));
		effectCaps.hasTexture = true;
	}
	
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
