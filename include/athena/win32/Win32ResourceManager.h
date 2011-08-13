#ifndef _WIN32RESOURCEMANAGER_H_
#define _WIN32RESOURCEMANAGER_H_

#include "athena/ResourceManager.h"
#include <d3d9.h>

namespace Athena
{
	class CWin32ResourceManager : public CResourceManager
	{
	public:
		static void				CreateInstance(IDirect3DDevice9*);
		static void				DestroyInstance();

		virtual std::string		MakeResourcePath(const char*) const;

		virtual void			ReleaseAllResources();

		virtual void			LoadTexture(const char*);
		virtual void			LoadFontDescriptor(const char*);

	protected:
								CWin32ResourceManager(IDirect3DDevice9*);
		virtual					~CWin32ResourceManager();

		IDirect3DDevice9*		m_device;
	};
}

#endif
