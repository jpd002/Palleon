#pragma once

#include "Dx11Effect.h"

namespace Athena
{
	class CDx11ShadowMapEffect : public CDx11Effect
	{
	public:
						CDx11ShadowMapEffect(ID3D11Device*, ID3D11DeviceContext*);
		virtual			~CDx11ShadowMapEffect();

		virtual void	UpdateConstants(const MaterialPtr&, const CMatrix4&, const CMatrix4&, const CMatrix4&, const CMatrix4&) override;

	private:
		uint32			m_worldMatrixOffset = -1;
		uint32			m_viewProjMatrixOffset = -1;
	};
};
