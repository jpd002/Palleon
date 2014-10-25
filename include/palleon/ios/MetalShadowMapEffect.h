#pragma once

#include "palleon/ios/MetalEffect.h"
#include "palleon/ios/MetalShadowMapEffectGenerator.h"

namespace Palleon
{
	class CMetalShadowMapEffect : public CMetalEffect
	{
	public:
		typedef CMetalShadowMapEffectGenerator::EFFECTCAPS EFFECTCAPS;
	
								CMetalShadowMapEffect(id<MTLDevice>, const EFFECTCAPS&);
		virtual					~CMetalShadowMapEffect();
		
		virtual void			UpdateConstants(const VIEWPORT_PARAMS&, CMaterial*, const CMatrix4&) override;
		virtual unsigned int	GetConstantsSize() const override;
		
	private:
		void					FillPipelinePixelFormats(MTLRenderPipelineDescriptor*) override;		
	};
}
