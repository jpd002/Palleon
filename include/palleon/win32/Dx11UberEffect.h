#pragma once

#include "Dx11UberEffectGenerator.h"
#include "Dx11Effect.h"

namespace Palleon
{
	class CDx11UberEffect : public CDx11Effect
	{
	public:
		typedef CDx11UberEffectGenerator::EFFECTCAPS EFFECTCAPS;

										CDx11UberEffect(ID3D11Device*, ID3D11DeviceContext*, const EFFECTCAPS&);
		virtual							~CDx11UberEffect();

		void							UpdateConstants(const VIEWPORT_PARAMS&, CMaterial*, const CMatrix4&) override;

	private:
		D3D11InputLayoutPtr				CreateInputLayout(const VERTEX_BUFFER_DESCRIPTOR&) override;

		uint32							m_meshColorOffset = -1;
		uint32							m_worldMatrixOffset = -1;
		uint32							m_viewProjMatrixOffset = -1;
		uint32							m_shadowViewProjMatrixOffset = -1;
		uint32							m_diffuseTextureMatrixOffset[CDx11UberEffectGenerator::MAX_DIFFUSE_SLOTS];

		EFFECTCAPS						m_effectCaps;
	};
};
