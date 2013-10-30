#pragma once

#include <memory>

namespace Athena
{
	class CResource
	{
	public:
		virtual			~CResource() {}
		virtual void	Load(const char*) = 0;
	};
	typedef std::shared_ptr<CResource> ResourcePtr;
}
