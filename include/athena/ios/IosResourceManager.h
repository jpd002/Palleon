#pragma once

#include "athena/resources/ResourceManager.h"

namespace Athena
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
