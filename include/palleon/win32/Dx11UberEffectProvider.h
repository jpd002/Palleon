#pragma once

#include <unordered_map>
#include "palleon/graphics/EffectProvider.h"
#include "palleon/win32/Dx11Effect.h"

namespace Palleon
{
	class CDx11UberEffectProvider : public CEffectProvider
	{
	public:
								CDx11UberEffectProvider(ID3D11Device*, ID3D11DeviceContext*);
		virtual					~CDx11UberEffectProvider();

		EffectPtr				GetEffectForRenderable(CMesh*, bool) override;

	private:
		typedef std::unordered_map<uint32, Dx11EffectPtr> EffectMap;

		ID3D11Device*			m_device;
		ID3D11DeviceContext*	m_deviceContext;

		EffectMap				m_effects;
	};
}
