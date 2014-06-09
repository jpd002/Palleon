#pragma once

#include "Types.h"
#include <unordered_map>
#include "palleon/EffectProvider.h"
#include "palleon/ios/MetalUberEffect.h"

namespace Palleon
{
	class CMetalUberEffectProvider : public CEffectProvider
	{
	public:
							CMetalUberEffectProvider(id<MTLDevice> device);
		virtual				~CMetalUberEffectProvider();
		
		virtual EffectPtr	GetEffectForRenderable(CMesh*, bool) override;
		
	private:
		typedef std::unordered_map<uint32, MetalEffectPtr> EffectMap;
		
		id<MTLDevice>		m_device;
		EffectMap			m_effects;
	};
}
