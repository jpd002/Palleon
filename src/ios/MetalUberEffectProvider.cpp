#include "TargetConditionals.h"
#if !TARGET_IPHONE_SIMULATOR

#include "palleon/ios/MetalUberEffectProvider.h"
#include "palleon/ios/MetalUberEffectGenerator.h"
#include "palleon/ios/MetalUberEffect.h"
#include "palleon/Mesh.h"

using namespace Palleon;

CMetalUberEffectProvider::CMetalUberEffectProvider(id<MTLDevice> device)
: m_device(device)
{

}

CMetalUberEffectProvider::~CMetalUberEffectProvider()
{

}

EffectPtr CMetalUberEffectProvider::GetEffectForRenderable(CMesh* mesh, bool hasShadowMap)
{
	auto vertexBuffer = mesh->GetVertexBuffer();
	auto material = mesh->GetMaterial();

	assert(vertexBuffer && material);

	const auto& descriptor = vertexBuffer->GetDescriptor();
	
	CMetalUberEffectGenerator::EFFECTCAPS effectCaps;
	memset(&effectCaps, 0, sizeof(effectCaps));
	
	effectCaps.hasNormal		= descriptor.HasVertexItem(VERTEX_ITEM_ID_NORMAL);
	effectCaps.hasTexCoord0		= descriptor.HasVertexItem(VERTEX_ITEM_ID_UV0);
	effectCaps.hasTexture		= material->GetTexture(0) ? true : false;
	effectCaps.hasShadowMap		= hasShadowMap && material->GetShadowReceiving();
	
	auto effectKey = *reinterpret_cast<uint32*>(&effectCaps);
	auto effectIterator = m_effects.find(effectKey);
	if(effectIterator == std::end(m_effects))
	{
		auto effect = std::make_shared<CMetalUberEffect>(m_device, effectCaps);
		m_effects.insert(std::make_pair(effectKey, effect));
		effectIterator = m_effects.find(effectKey);
	}
	
	return effectIterator->second;
}

#endif
