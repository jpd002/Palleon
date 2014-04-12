#pragma once

#include <memory>

namespace Palleon
{
	class CEffect
	{
	public:
		virtual					~CEffect() {};
	};

	typedef std::shared_ptr<CEffect> EffectPtr;
}
