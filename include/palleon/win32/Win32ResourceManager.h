#pragma once

#include "palleon/resources/ResourceManager.h"

namespace Palleon
{
	class CWin32ResourceManager : public CResourceManager
	{
	public:
		static void				CreateInstance();
		static void				DestroyInstance();

		virtual std::string		MakeResourcePath(const std::string&) const override;
		virtual StreamPtr		MakeResourceStream(const std::string&) const override;

	protected:
								CWin32ResourceManager();
		virtual					~CWin32ResourceManager();
	};
}
