#pragma once

#include "../resources/ResourceManager.h"

namespace Palleon
{
	class CAndroidResourceManager : public CResourceManager
	{
	public:
		static void					CreateInstance();
		static void					DestroyInstance();
		
		virtual std::string			MakeResourcePath(const std::string&) const override;
		virtual StreamPtr			MakeResourceStream(const std::string&) const override;
		
	protected:
									CAndroidResourceManager();
		virtual						~CAndroidResourceManager();
	};
}
