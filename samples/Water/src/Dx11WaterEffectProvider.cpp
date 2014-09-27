#include "Dx11WaterEffectProvider.h"
#include "Dx11WaterEffect.h"

CDx11WaterEffectProvider::CDx11WaterEffectProvider(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	m_effect = std::make_shared<CDx11WaterEffect>(device, deviceContext);
}

Palleon::EffectPtr CDx11WaterEffectProvider::GetEffectForRenderable(Palleon::CMesh*, bool)
{
	return m_effect;
}
