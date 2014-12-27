#pragma once

#include "palleon/graphics/SharedGraphicContext.h"

namespace Palleon
{
	class CDx11SharedGraphicContext : public CSharedGraphicContext
	{
	public:
						CDx11SharedGraphicContext();
		virtual			~CDx11SharedGraphicContext();

		virtual void	Flush() override;
	};
}
