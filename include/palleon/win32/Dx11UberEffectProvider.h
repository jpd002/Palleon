#pragma once

#include <unordered_map>
#include "athena/EffectProvider.h"
#include "athena/win32/Dx11Effect.h"

namespace Athena
{
	class CDx11UberEffectProvider : public CEffectProvider
	{
	public:
								CDx11UberEffectProvider(ID3D11Device*, ID3D11DeviceContext*);
		virtual					~CDx11UberEffectProvider();

		virtual EffectPtr		GetEffectForRenderable(CMesh*, bool hasShadowMap) override;

	private:
		typedef std::unordered_map<uint32, Dx11EffectPtr> EffectMap;

		ID3D11Device*			m_device;
		ID3D11DeviceContext*	m_deviceContext;

		EffectMap				m_effects;
	};
}
