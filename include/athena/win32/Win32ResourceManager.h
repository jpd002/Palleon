#ifndef _WIN32RESOURCEMANAGER_H_
#define _WIN32RESOURCEMANAGER_H_

#include "athena/ResourceManager.h"

namespace Athena
{
	class CWin32ResourceManager : public CResourceManager
	{
	public:
		static void				CreateInstance();
		static void				DestroyInstance();

		virtual std::string		MakeResourcePath(const char*) const;

	protected:
								CWin32ResourceManager();
		virtual					~CWin32ResourceManager();
	};
}

#endif
