#include "palleon/win32/Dx11UberEffectProvider.h"
#include "palleon/win32/Dx11UberEffectGenerator.h"
#include "palleon/win32/Dx11VertexBuffer.h"
#include "palleon/win32/Dx11UberEffect.h"
#include "palleon/Mesh.h"

using namespace Palleon;

CDx11UberEffectProvider::CDx11UberEffectProvider(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
: m_device(device)
, m_deviceContext(deviceContext)
{

}

CDx11UberEffectProvider::~CDx11UberEffectProvider()
{

}

EffectPtr CDx11UberEffectProvider::GetEffectForRenderable(CMesh* mesh, bool hasShadowMap)
{
	auto vertexBufferGen = std::static_pointer_cast<CDx11VertexBuffer>(mesh->GetVertexBuffer());
	assert(vertexBufferGen);

	const auto& descriptor = vertexBufferGen->GetDescriptor();

	auto material = mesh->GetMaterial();
	assert(material != NULL);

	CDx11UberEffectGenerator::EFFECTCAPS effectCaps;
	memset(&effectCaps, 0, sizeof(effectCaps));

	if(descriptor.HasVertexItem(VERTEX_ITEM_ID_COLOR))
	{
		effectCaps.hasVertexColor = true;
	}

	effectCaps.hasShadowMap = hasShadowMap && material->GetShadowReceiving();

	for(unsigned int i = 0; i < CDx11UberEffectGenerator::MAX_DIFFUSE_SLOTS; i++)
	{
		if(material->GetTexture(i))
		{
			auto textureCoordSource = material->GetTextureCoordSource(i);
			if(textureCoordSource == TEXTURE_COORD_UV0 && ((descriptor.HasVertexItem(VERTEX_ITEM_ID_UV0)) == 0)) continue;

			effectCaps.setHasDiffuseMap(i, true);
			effectCaps.setDiffuseMapCoordSrc(i, textureCoordSource);
			if(i != 0)
			{
				unsigned int combineMode = DIFFUSE_MAP_COMBINE_MODULATE;
				switch(material->GetTextureCombineMode(i))
				{
				case TEXTURE_COMBINE_MODULATE:
					combineMode = DIFFUSE_MAP_COMBINE_MODULATE;
					break;
				case TEXTURE_COMBINE_LERP:
					combineMode = DIFFUSE_MAP_COMBINE_LERP;
					break;
				case TEXTURE_COMBINE_ADD:
					combineMode = DIFFUSE_MAP_COMBINE_ADD;
					break;
				}
				effectCaps.setDiffuseMapCombineMode(i, combineMode);
			}
		}
	}

	//Find the proper effect
	uint32 effectKey = *reinterpret_cast<uint32*>(&effectCaps);
	auto effectIterator = m_effects.find(effectKey);
	if(effectIterator == std::end(m_effects))
	{
		auto effect = std::make_shared<CDx11UberEffect>(m_device, m_deviceContext, effectCaps);
		m_effects.insert(std::make_pair(effectKey, effect));
		effectIterator = m_effects.find(effectKey);
	}

	return effectIterator->second;
}
