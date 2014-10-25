#pragma once

#include "palleon/ios/MetalEffect.h"
#include "palleon/ios/MetalUberEffectGenerator.h"

namespace Palleon
{
	class CMetalUberEffect : public CMetalEffect
	{
	public:
		typedef CMetalUberEffectGenerator::EFFECTCAPS EFFECTCAPS;
		
								CMetalUberEffect(id<MTLDevice>, const EFFECTCAPS&);
		virtual					~CMetalUberEffect();
		
		virtual void			UpdateConstants(const VIEWPORT_PARAMS&, CMaterial*, const CMatrix4&) override;
		virtual unsigned int	GetConstantsSize() const override;
		
	private:
		void					FillPipelinePixelFormats(MTLRenderPipelineDescriptor*) override;
		
		uint32					m_worldMatrixOffset = -1;
		uint32					m_viewProjMatrixOffset = -1;
		uint32					m_shadowViewProjMatrixOffset = -1;
		uint32					m_meshColorOffset = -1;
		uint32					m_constantsSize = 0;
	};
}
