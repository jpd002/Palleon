#include "athena/Mesh.h"
#include "athena/ios/IosUberEffectProvider.h"
#include "athena/ios/IosUberEffect.h"

using namespace Athena;

static const unsigned int g_textureCombineMode[TEXTURE_COMBINE_MODE_MAX] =
{
	DIFFUSE_MAP_COMBINE_MODULATE,
	DIFFUSE_MAP_COMBINE_LERP,
	DIFFUSE_MAP_COMBINE_ADD,
};

CIosUberEffectProvider::CIosUberEffectProvider()
{
	
}

CIosUberEffectProvider::~CIosUberEffectProvider()
{
	
}

EffectPtr CIosUberEffectProvider::GetEffectForRenderable(CMesh* mesh, bool hasShadowMap)
{
	auto vertexBuffer = mesh->GetVertexBuffer();
	const auto& descriptor = vertexBuffer->GetDescriptor();
	
	auto material = mesh->GetMaterial();
	assert(material);

	CIosUberEffectGenerator::EFFECTCAPS effectCaps;
	memset(&effectCaps, 0, sizeof(effectCaps));
	
	if(descriptor.vertexFlags & VERTEX_BUFFER_HAS_COLOR)
	{
		effectCaps.hasVertexColor = true;
	}
	
	effectCaps.hasShadowMap = hasShadowMap && material->GetShadowReceiving();
	
	for(unsigned int i = 0; i < CIosUberEffect::MAX_DIFFUSE_SLOTS; i++)
	{
		if(material->GetTexture(i))
		{
			effectCaps.setHasDiffuseMap(i, true);
			effectCaps.setDiffuseMapCoordSrc(i, material->GetTextureCoordSource(i));
			if(i != 0)
			{
				unsigned int combineMode = g_textureCombineMode[material->GetTextureCombineMode(i)];
				effectCaps.setDiffuseMapCombineMode(i, combineMode);
			}
		}
	}
	
	uint32 effectKey = *reinterpret_cast<uint32*>(&effectCaps);
	auto effectIterator = m_effects.find(effectKey);
	if(effectIterator == std::end(m_effects))
	{
		auto effect = std::make_shared<CIosUberEffect>(effectCaps);
		m_effects.insert(std::make_pair(effectKey, effect));
		effectIterator = m_effects.find(effectKey);
	}
	
	return effectIterator->second;
}
