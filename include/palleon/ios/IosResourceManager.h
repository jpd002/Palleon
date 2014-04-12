#pragma once

#include "palleon/resources/ResourceManager.h"

namespace Palleon
{
	class CIosResourceManager : public CResourceManager
	{
	public:
		static void					CreateInstance();
		static void					DestroyInstance();
		
		virtual std::string			MakeResourcePath(const std::string&) const override;
		
	protected:
									CIosResourceManager();
		virtual						~CIosResourceManager();
	};
}
