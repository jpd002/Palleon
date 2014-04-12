#pragma once

#include "palleon/gles/GlEsEffect.h"
#include "palleon/gles/GlEsUberEffectGenerator.h"

namespace Palleon
{
	class CGlEsUberEffect : public CGlEsEffect
	{
	public:
		enum
		{
			MAX_DIFFUSE_SLOTS = 5
		};
		
		typedef CGlEsUberEffectGenerator::EFFECTCAPS EFFECTCAPS;
		
						CGlEsUberEffect(const EFFECTCAPS&);
		virtual			~CGlEsUberEffect();

		virtual void	UpdateConstants(const MaterialPtr&, const CMatrix4&, const CMatrix4&, const CMatrix4&, const CMatrix4&) override;
				
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
