#pragma once

#include "Dx11UberEffectGenerator.h"
#include "Dx11Effect.h"

namespace Athena
{
	class CDx11UberEffect : public CDx11Effect
	{
	public:
		typedef CDx11UberEffectGenerator::EFFECTCAPS EFFECTCAPS;

						CDx11UberEffect(ID3D11Device*, ID3D11DeviceContext*, const EFFECTCAPS&);
		virtual			~CDx11UberEffect();

		virtual void	UpdateConstants(const MaterialPtr&, const CMatrix4&, const CMatrix4&, const CMatrix4&, const CMatrix4&) override;

	private:
		uint32			m_meshColorOffset = -1;
		uint32			m_worldMatrixOffset = -1;
		uint32			m_viewProjMatrixOffset = -1;
		uint32			m_shadowViewProjMatrixOffset = -1;
		uint32			m_diffuseTextureMatrixOffset[CDx11UberEffectGenerator::MAX_DIFFUSE_SLOTS];

		EFFECTCAPS		m_effectCaps;
	};
};
