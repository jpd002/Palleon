#pragma once

#include "palleon/graphics/Effect.h"

namespace Palleon
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
