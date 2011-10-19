#ifndef _ILAYOUTABLE_H_
#define	_ILAYOUTABLE_H_

#include <memory>
#include "athena/Vector2.h"

namespace Athena
{
	class ILayoutable
	{
	public:
		virtual				~ILayoutable() {}

		virtual void		SetPosition(const CVector2&) = 0;
		virtual void		SetSize(const CVector2&) = 0;
	};

	typedef std::tr1::shared_ptr<ILayoutable> LayoutablePtr;
}

#endif
