#pragma once

#include <unordered_map>
#include "athena/FontDescriptor.h"
#include "athena/NinePatchDescriptor.h"
#include "athena/EmitterDescriptor.h"
#include "athena/Texture.h"
#include "Types.h"

namespace Athena
{
	class CResourceManager
	{
	public:
		virtual						~CResourceManager();
		
		static CResourceManager&	GetInstance();

		virtual std::string			MakeResourcePath(const char*) const = 0;
		
		TexturePtr					GetTexture(const char*) const;
		void						LoadTexture(const char*, const char* = nullptr);
		void						ReleaseTexture(const char*);

		const CFontDescriptor*		GetFontDescriptor(const char*) const;
		void						LoadFontDescriptor(const char*, const char* = nullptr);
		void						ReleaseFontDescriptor(const char*);
		
		const CNinePatchDescriptor*	GetNinePatchDescriptor(const char*) const;
		void						LoadNinePatchDescriptor(const char*, const char* = nullptr);
		void						ReleaseNinePatchDescriptor(const char*);

		const CEmitterDescriptor*	GetEmitterDescriptor(const char*) const;
		void						LoadEmitterDescriptor(const char*, const char* = nullptr);
		void						ReleaseEmitterDescriptor(const char*);

	protected:
		typedef std::unordered_map<unsigned int, TexturePtr> TextureMap;
		typedef std::unordered_map<unsigned int, CFontDescriptor*> FontDescriptorMap;
		typedef std::unordered_map<unsigned int, CNinePatchDescriptor*> NinePatchDescriptorMap;
		typedef std::unordered_map<unsigned int, CEmitterDescriptor*> EmitterDescriptorMap;

									CResourceManager();

		static uint32				MakeCrc(const char*);

		static CResourceManager*	m_instance;

		TextureMap					m_textures;
		FontDescriptorMap			m_fontDescriptors;
		NinePatchDescriptorMap		m_ninePatchDescriptors;
		EmitterDescriptorMap		m_emitterDescriptors;
	};
}
