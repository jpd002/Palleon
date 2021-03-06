#include <assert.h>
#include <boost/scoped_ptr.hpp>
#include "palleon/resources/FontDescriptor.h"
#include "xml/Node.h"
#include "xml/Parser.h"
#include "xml/Utils.h"

using namespace Palleon;

CFontDescriptor::CFontDescriptor()
: m_textureWidth(0)
, m_textureHeight(0)
, m_lineHeight(0)
{

}

CFontDescriptor::~CFontDescriptor()
{

}

const char* CFontDescriptor::GetTextureName() const
{
	return m_textureName.c_str();
}

int CFontDescriptor::GetTextureWidth() const
{
	return m_textureWidth;
}

int CFontDescriptor::GetTextureHeight() const
{
	return m_textureHeight;
}

int CFontDescriptor::GetLineHeight() const
{
	return m_lineHeight;
}

#ifdef _MSC_VER
//Don't know what's going on here, doesn't seem to play well with inlining
__declspec(noinline)
#endif
CFontDescriptor::GLYPHINFO CFontDescriptor::GetGlyphInfo(unsigned int charId) const
{
	auto glyphIterator(m_glyphInfos.find(charId));
	assert(glyphIterator != std::end(m_glyphInfos));
	return glyphIterator->second;
}

void CFontDescriptor::Load(Framework::CStream& inputStream)
{
	m_glyphInfos.clear();

	boost::scoped_ptr<Framework::Xml::CNode> document(Framework::Xml::CParser::ParseDocument(inputStream));

	//Get common info
	{
		Framework::Xml::CNode* commonNode = document->Select("font/common");
		Framework::Xml::GetAttributeIntValue(commonNode, "lineHeight", &m_lineHeight);
		Framework::Xml::GetAttributeIntValue(commonNode, "scaleW", &m_textureWidth);
		Framework::Xml::GetAttributeIntValue(commonNode, "scaleH", &m_textureHeight);
	}

	//Get page info
	{
		auto pageNodes = document->SelectNodes("font/pages/page");
		for(auto pageNodeIterator(std::begin(pageNodes)); pageNodeIterator != std::end(pageNodes); pageNodeIterator++)
		{
			Framework::Xml::CNode* pageNode(*pageNodeIterator);

			int pageId = 0;

			Framework::Xml::GetAttributeIntValue(pageNode, "id", &pageId);
			std::string pageFile = Framework::Xml::GetAttributeStringValue(pageNode, "file");

			PAGEINFO pageInfo;
			pageInfo.file = pageFile;

			assert(m_pageInfos.find(pageId) == m_pageInfos.end());
			m_pageInfos[pageId] = pageInfo;
		}
	}

	//Get char info
	{
		auto charNodes = document->SelectNodes("font/chars/char");
		for(auto charNodeIterator(std::begin(charNodes)); charNodeIterator != std::end(charNodes); charNodeIterator++)
		{
			Framework::Xml::CNode* charNode(*charNodeIterator);
			int charId = 0;
			int charX = 0;
			int charY = 0;
			int charWidth = 0;
			int charHeight = 0;
			int charXOffset = 0;
			int charYOffset = 0;
			int charXAdvance = 0;
			int pageId = 0;

			Framework::Xml::GetAttributeIntValue(charNode, "id", &charId);
			Framework::Xml::GetAttributeIntValue(charNode, "x", &charX);
			Framework::Xml::GetAttributeIntValue(charNode, "y", &charY);
			Framework::Xml::GetAttributeIntValue(charNode, "width", &charWidth);
			Framework::Xml::GetAttributeIntValue(charNode, "height", &charHeight);
			Framework::Xml::GetAttributeIntValue(charNode, "xoffset", &charXOffset);
			Framework::Xml::GetAttributeIntValue(charNode, "yoffset", &charYOffset);
			Framework::Xml::GetAttributeIntValue(charNode, "xadvance", &charXAdvance);
			Framework::Xml::GetAttributeIntValue(charNode, "page", &pageId);

			GLYPHINFO glyphInfo;
			glyphInfo.x = charX;
			glyphInfo.y = charY;
			glyphInfo.dx = charWidth;
			glyphInfo.dy = charHeight;
			glyphInfo.xoffset = charXOffset;
			glyphInfo.yoffset = charYOffset;
			glyphInfo.xadvance = charXAdvance;
			glyphInfo.pageId = pageId;

			assert(m_glyphInfos.find(charId) == m_glyphInfos.end());
			m_glyphInfos[charId] = glyphInfo;
		}
	}

	{
		const PAGEINFO& pageInfo(m_pageInfos[0]);
		std::string textureName = std::string(pageInfo.file.begin(), pageInfo.file.begin() + pageInfo.file.find('.'));
		m_textureName = textureName;
		m_textureName += ".png";
	}
}
