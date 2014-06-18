#pragma once

#include "Types.h"
#include <unordered_map>
#include "palleon/EffectProvider.h"
#include "palleon/ios/MetalEffect.h"

namespace Palleon
{
	class CMetalShadowMapEffectProvider : public CEffectProvider
	{
	public:
							CMetalShadowMapEffectProvider(id<MTLDevice> device);
		virtual				~CMetalShadowMapEffectProvider();
		
		virtual EffectPtr	GetEffectForRenderable(CMesh*, bool) override;
		
	private:
		typedef std::unordered_map<uint32, MetalEffectPtr> EffectMap;
		
		id<MTLDevice>		m_device;
		EffectMap			m_effects;
	};
}
