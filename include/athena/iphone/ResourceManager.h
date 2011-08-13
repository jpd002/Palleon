#ifndef _RESOURCEMANAGER_H_
#define _RESOURCEMANAGER_H_

#include <unordered_map>
#include "FontDescriptor.h"
#include "OpenGlDefs.h"

class CResourceManager
{
public:
	virtual						~CResourceManager();
	
	void						ReleaseAllResources();
	
	static CResourceManager&	GetInstance();
	
	void						LoadTexture(const char*);
	void						LoadFontDescriptor(const char*);
	
	GLuint						GetTexture(const char*) const;
	const CFontDescriptor*		GetFontDescriptor(const char*) const;
	
private:
								CResourceManager();

	typedef std::tr1::unordered_map<unsigned int, GLuint> TextureMap;
	typedef std::tr1::unordered_map<unsigned int, CFontDescriptor*> FontDescriptorMap;
	
	TextureMap					m_textures;
	FontDescriptorMap			m_fontDescriptors;
};

#endif
