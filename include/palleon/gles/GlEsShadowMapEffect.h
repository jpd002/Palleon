#pragma once

#include "palleon/gles/GlEsEffect.h"

namespace Palleon
{
	class CGlEsShadowMapEffect : public CGlEsEffect
	{
	public:
						CGlEsShadowMapEffect();
		virtual			~CGlEsShadowMapEffect();
		
		virtual void	UpdateConstants(const MaterialPtr&, const CMatrix4&, const CMatrix4&, const CMatrix4&, const CMatrix4&) override;
		
	private:
		GLuint			m_viewProjMatrixHandle = -1;
		GLuint			m_worldMatrixHandle = -1;		
	};
}