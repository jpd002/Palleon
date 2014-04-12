#pragma once

#include <memory>

namespace Palleon
{
	class CResource
	{
	public:
		virtual			~CResource() {}
		virtual void	Load(const char*) = 0;
	};
	typedef std::shared_ptr<CResource> ResourcePtr;
}
