#ifndef _RESOURCEMANAGER_H_
#define _RESOURCEMANAGER_H_

#include <unordered_map>
#include "athena/FontDescriptor.h"
#include "athena/Texture.h"
#include "Types.h"

namespace Athena
{
	class CResourceManager
	{
	public:
		virtual						~CResourceManager();
		
		virtual void				ReleaseAllResources() = 0;
		
		static CResourceManager&	GetInstance();

		virtual std::string			MakeResourcePath(const char*) const = 0;
		
		TexturePtr					GetTexture(const char*) const;
		virtual void				LoadTexture(const char*) = 0;
		void						ReleaseTexture(const char*);

		const CFontDescriptor*		GetFontDescriptor(const char*) const;
		virtual void				LoadFontDescriptor(const char*) = 0;
		void						ReleaseFontDescriptor(const char*);
		
	protected:
		typedef std::tr1::unordered_map<unsigned int, TexturePtr> TextureMap;
		typedef std::tr1::unordered_map<unsigned int, CFontDescriptor*> FontDescriptorMap;

									CResourceManager();

		static uint32				MakeCrc(const char*);

		static CResourceManager*	m_instance;

		TextureMap					m_textures;
		FontDescriptorMap			m_fontDescriptors;
	};
}

#endif
