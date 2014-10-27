#pragma once

#include "palleon/graphics/EffectProvider.h"

namespace Palleon
{
	template<typename EffectType>
	class CGenericEffectProvider : public Palleon::CEffectProvider
	{
	public:
		CGenericEffectProvider()
		{
			m_effect = std::make_shared<EffectType>();
		}

		Palleon::EffectPtr GetEffectForRenderable(Palleon::CMesh*, bool) override
		{
			return m_effect;
		}

	private:
		Palleon::EffectPtr m_effect;
	};
}
