#pragma once

#include <memory>

namespace Palleon
{
	template <typename TargetType>
	class IAnimation
	{
	public:
		virtual				~IAnimation() {}
		virtual void		Animate(TargetType* target, float t) const = 0;
		virtual float		GetLength() const = 0;
	};
}
