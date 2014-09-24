#pragma once

#include <unordered_map>
#include <string>
#include "Resource.h"

namespace Palleon
{
	class CFontDescriptor : public CResource
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
		int				GetLineHeight() const;

		GLYPHINFO		GetGlyphInfo(unsigned int) const;
			
		void			Load(Framework::CStream&) override;
		
	private:
		typedef std::unordered_map<unsigned int, PAGEINFO> PageInfoMap;
		typedef std::unordered_map<unsigned int, GLYPHINFO> GlyphInfoMap;
		
		int				m_textureWidth;
		int				m_textureHeight;
		int				m_lineHeight;
		PageInfoMap		m_pageInfos;
		GlyphInfoMap	m_glyphInfos;
		std::string		m_textureName;
	};
}
