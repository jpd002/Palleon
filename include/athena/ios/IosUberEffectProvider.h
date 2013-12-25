#pragma once

#include "athena/EffectProvider.h"

namespace Athena
{
	class CIosUberEffectProvider : public CEffectProvider
	{
	public:
							CIosUberEffectProvider();
		virtual				~CIosUberEffectProvider();
		
		virtual EffectPtr	GetEffectForRenderable(CMesh*, bool hasShadowMap) override;
		
	private:
		typedef std::map<uint32, EffectPtr> EffectMap;
		
		EffectMap			m_effects;
	};
}