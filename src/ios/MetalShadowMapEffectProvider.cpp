#include "TargetConditionals.h"
#if !TARGET_IPHONE_SIMULATOR

#include "palleon/ios/MetalShadowMapEffectProvider.h"
#include "palleon/ios/MetalShadowMapEffectGenerator.h"
#include "palleon/ios/MetalShadowMapEffect.h"
#include "palleon/Mesh.h"

using namespace Palleon;

CMetalShadowMapEffectProvider::CMetalShadowMapEffectProvider(id<MTLDevice> device)
: m_device(device)
{

}

CMetalShadowMapEffectProvider::~CMetalShadowMapEffectProvider()
{

}

EffectPtr CMetalShadowMapEffectProvider::GetEffectForRenderable(Palleon::CMesh* mesh, bool)
{
	auto vertexBuffer = mesh->GetVertexBuffer();
	auto material = mesh->GetMaterial();

	assert(vertexBuffer && material);

	const auto& descriptor = vertexBuffer->GetDescriptor();
	
	CMetalShadowMapEffectGenerator::EFFECTCAPS effectCaps;
	memset(&effectCaps, 0, sizeof(effectCaps));
	
	effectCaps.hasNormal		= descriptor.HasVertexItem(VERTEX_ITEM_ID_NORMAL);
	effectCaps.hasTexCoord0		= descriptor.HasVertexItem(VERTEX_ITEM_ID_UV0);
	
	auto effectKey = *reinterpret_cast<uint32*>(&effectCaps);
	auto effectIterator = m_effects.find(effectKey);
	if(effectIterator == std::end(m_effects))
	{
		auto effect = std::make_shared<CMetalShadowMapEffect>(m_device, effectCaps);
		m_effects.insert(std::make_pair(effectKey, effect));
		effectIterator = m_effects.find(effectKey);
	}
	
	return effectIterator->second;
}

#endif
