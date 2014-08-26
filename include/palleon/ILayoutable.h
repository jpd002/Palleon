#pragma once

#include <memory>
#include "palleon/Vector2.h"
#include "palleon/Vector3.h"

namespace Palleon
{
	class ILayoutable
	{
	public:
		virtual				~ILayoutable() {}

		virtual void		SetPosition(const CVector3&) = 0;
		virtual void		SetSize(const CVector2&) = 0;
	};

	typedef std::shared_ptr<ILayoutable> LayoutablePtr;
}
