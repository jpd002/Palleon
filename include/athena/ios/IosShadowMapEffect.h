#pragma once

#include "athena/ios/IosEffect.h"

namespace Athena
{
	class CIosShadowMapEffect : public CIosEffect
	{
	public:
						CIosShadowMapEffect();
		virtual			~CIosShadowMapEffect();
		
		virtual void	UpdateConstants(const MaterialPtr&, const CMatrix4&, const CMatrix4&, const CMatrix4&) override;
		
	private:
		GLuint			m_viewProjMatrixHandle = -1;
		GLuint			m_worldMatrixHandle = -1;		
	};
}