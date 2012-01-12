#ifndef _ATHENA_FONTDESCRIPTOR_H_
#define _ATHENA_FONTDESCRIPTOR_H_

#include <unordered_map>
#include <string>

namespace Athena
{
	class CFontDescriptor
	{
	public:
		struct PAGEINFO
		{
			std::string file;
		};
	    
		struct GLYPHINFO
		{
			float x;
			float y;
			float dx;
			float dy;
			float xoffset;
			float yoffset;
			float xadvance;
			int pageId;
		};
		
						CFontDescriptor();
		virtual			~CFontDescriptor();
		
		const char*		GetTextureName() const;
		int				GetTextureWidth() const;
		int				GetTextureHeight() const;
		int             GetLineHeight() const;

		GLYPHINFO		GetGlyphInfo(unsigned int) const;
			
		void			Load(const char*);
		
	private:
		typedef std::tr1::unordered_map<unsigned int, PAGEINFO> PageInfoMap;
		typedef std::tr1::unordered_map<unsigned int, GLYPHINFO> GlyphInfoMap;
		
		int				m_textureWidth;
		int				m_textureHeight;
		int             m_lineHeight;
		PageInfoMap     m_pageInfos;
		GlyphInfoMap	m_glyphInfos;
		std::string		m_textureName;
	};
}

#endif
