#pragma once

#include <memory>

namespace Athena
{
	class CEffect
	{
	public:
		virtual					~CEffect() {};
	};

	typedef std::shared_ptr<CEffect> EffectPtr;
}
