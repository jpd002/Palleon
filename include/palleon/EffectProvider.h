#pragma once

#include "Effect.h"

namespace Athena
{
	class CMesh;

	class CEffectProvider
	{
	public:
		virtual					~CEffectProvider() {};

		virtual EffectPtr		GetEffectForRenderable(CMesh*, bool hasShadowMap) = 0;
	};

	typedef std::shared_ptr<CEffectProvider> EffectProviderPtr;
}
