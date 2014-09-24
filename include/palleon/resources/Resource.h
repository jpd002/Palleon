#pragma once

#include <memory>
#include "Stream.h"

namespace Palleon
{
	class CResource
	{
	public:
		virtual			~CResource() {}
		virtual void	Load(Framework::CStream&) = 0;
	};
	typedef std::shared_ptr<CResource> ResourcePtr;
}
