#pragma once

#include "athena/ResourceManager.h"

namespace Athena
{
	class CIosResourceManager : public CResourceManager
	{
	public:
		static void					CreateInstance();
		static void					DestroyInstance();
		
		std::string					MakeResourcePath(const char*) const;
		
	protected:
									CIosResourceManager();
		virtual						~CIosResourceManager();
	};
}
