#pragma once

#include "athena/Ios/IosEffect.h"
#include "athena/Ios/IosUberEffectGenerator.h"

namespace Athena
{
	class CIosUberEffect : public CIosEffect
	{
	public:
		enum
		{
			MAX_DIFFUSE_SLOTS = 5
		};
		
		typedef CIosUberEffectGenerator::EFFECTCAPS EFFECTCAPS;
		
						CIosUberEffect(const EFFECTCAPS&);
		virtual			~CIosUberEffect();

		virtual void	UpdateConstants(const MaterialPtr&, const CMatrix4&, const CMatrix4&, const CMatrix4&) override;
				
	private:
		GLuint			m_viewProjMatrixHandle = -1;
		GLuint			m_worldMatrixHandle = -1;
		GLuint			m_shadowViewProjMatrixHandle = -1;
		GLuint			m_meshColorHandle = -1;
			
		GLuint			m_diffuseTextureHandle[MAX_DIFFUSE_SLOTS];
		GLuint			m_diffuseTextureMatrixHandle[MAX_DIFFUSE_SLOTS];
		GLuint			m_shadowTextureHandle = -1;
	};
}
