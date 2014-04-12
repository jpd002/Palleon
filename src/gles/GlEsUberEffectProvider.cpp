#include "palleon/Mesh.h"
#include "palleon/gles/GlEsUberEffectProvider.h"
#include "palleon/gles/GlEsUberEffect.h"

using namespace Palleon;

static const unsigned int g_textureCombineMode[TEXTURE_COMBINE_MODE_MAX] =
{
	DIFFUSE_MAP_COMBINE_MODULATE,
	DIFFUSE_MAP_COMBINE_LERP,
	DIFFUSE_MAP_COMBINE_ADD,
};

CGlEsUberEffectProvider::CGlEsUberEffectProvider()
{
	
}

CGlEsUberEffectProvider::~CGlEsUberEffectProvider()
{
	
}

EffectPtr CGlEsUberEffectProvider::GetEffectForRenderable(CMesh* mesh, bool hasShadowMap)
{
	auto vertexBuffer = mesh->GetVertexBuffer();
	const auto& descriptor = vertexBuffer->GetDescriptor();
	
	auto material = mesh->GetMaterial();
	assert(material);

	CGlEsUberEffectGenerator::EFFECTCAPS effectCaps;
	memset(&effectCaps, 0, sizeof(effectCaps));
	
	if(descriptor.HasVertexItem(VERTEX_ITEM_ID_COLOR))
	{
		effectCaps.hasVertexColor = true;
	}
	
	effectCaps.hasShadowMap = hasShadowMap && material->GetShadowReceiving();
	
	for(unsigned int i = 0; i < CGlEsUberEffect::MAX_DIFFUSE_SLOTS; i++)
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
		auto effect = std::make_shared<CGlEsUberEffect>(effectCaps);
		m_effects.insert(std::make_pair(effectKey, effect));
		effectIterator = m_effects.find(effectKey);
	}
	
	return effectIterator->second;
}
