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
		
		virtual void			UpdateConstants(void*, const METALVIEWPORT_PARAMS&, CMaterial*, const CMatrix4&) override;
		virtual unsigned int	GetConstantsSize() const override;
		
	private:
	
	};
}
