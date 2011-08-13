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
		
		virtual void				LoadTexture(const char*) = 0;
		virtual void				LoadFontDescriptor(const char*) = 0;
		
		TexturePtr					GetTexture(const char*) const;
		const CFontDescriptor*		GetFontDescriptor(const char*) const;
		
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
