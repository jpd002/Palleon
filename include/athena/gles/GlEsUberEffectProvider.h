#pragma once

#include "athena/EffectProvider.h"

namespace Athena
{
	class CGlEsUberEffectProvider : public CEffectProvider
	{
	public:
							CGlEsUberEffectProvider();
		virtual				~CGlEsUberEffectProvider();
		
		virtual EffectPtr	GetEffectForRenderable(CMesh*, bool hasShadowMap) override;
		
	private:
		typedef std::map<uint32, EffectPtr> EffectMap;
		
		EffectMap			m_effects;
	};
}
