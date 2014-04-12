#pragma once

#include "athena/resources/ResourceManager.h"

namespace Athena
{
	class CWin32ResourceManager : public CResourceManager
	{
	public:
		static void				CreateInstance();
		static void				DestroyInstance();

		virtual std::string		MakeResourcePath(const std::string&) const override;

	protected:
								CWin32ResourceManager();
		virtual					~CWin32ResourceManager();
	};
}
