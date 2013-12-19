#pragma once

#include "Dx11EffectGenerator.h"
#include "Dx11Effect.h"

namespace Athena
{
	class CDx11UberEffect : public CDx11Effect
	{
	public:
		typedef CDx11EffectGenerator::EFFECTCAPS EFFECTCAPS;

						CDx11UberEffect(ID3D11Device*, ID3D11DeviceContext*, const EFFECTCAPS&);
		virtual			~CDx11UberEffect();

		virtual void	UpdateConstants(const MaterialPtr&, const CMatrix4&, const CMatrix4&, const CMatrix4&) override;

	private:
		enum 
		{
			//This should be in the effect generator
			MAX_DIFFUSE_SLOTS = 5,
		};

		uint32			m_meshColorOffset = -1;
		uint32			m_worldMatrixOffset = -1;
		uint32			m_viewProjMatrixOffset = -1;
		uint32			m_shadowViewProjMatrixOffset = -1;
		uint32			m_diffuseTextureMatrixOffset[MAX_DIFFUSE_SLOTS];

		EFFECTCAPS		m_effectCaps;
	};
};
