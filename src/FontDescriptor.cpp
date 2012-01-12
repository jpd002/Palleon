#include <assert.h>
#include <boost/scoped_ptr.hpp>
#include "athena/FontDescriptor.h"
#include "xml/Node.h"
#include "xml/Parser.h"
#include "xml/Utils.h"
#include "StdStream.h"

using namespace Athena;

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

CFontDescriptor::GLYPHINFO CFontDescriptor::GetGlyphInfo(unsigned int charId) const
{
	GlyphInfoMap::const_iterator glyphIterator(m_glyphInfos.find(charId));
	assert(glyphIterator != m_glyphInfos.end());
	return glyphIterator->second;
}

void CFontDescriptor::Load(const char* path)
{
    m_glyphInfos.clear();
    
    Framework::CStdStream inputStream(path, "rb");
    boost::scoped_ptr<Framework::Xml::CNode> document(Framework::Xml::CParser::ParseDocument(&inputStream));
    
    //Get common info
    {
        Framework::Xml::CNode* commonNode = document->Select("font/common");
        Framework::Xml::GetAttributeIntValue(commonNode, "lineHeight", &m_lineHeight);
		Framework::Xml::GetAttributeIntValue(commonNode, "scaleW", &m_textureWidth);
		Framework::Xml::GetAttributeIntValue(commonNode, "scaleH", &m_textureHeight);
    }
    
    //Get page info
    {
        Framework::Xml::CNode::NodeList pageNodes = document->SelectNodes("font/pages/page");
        for(Framework::Xml::CNode::NodeIterator pageNodeIterator(pageNodes.begin());
            pageNodeIterator != pageNodes.end(); pageNodeIterator++)
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
        Framework::Xml::CNode::NodeList charNodes = document->SelectNodes("font/chars/char");
        for(Framework::Xml::CNode::NodeIterator charNodeIterator(charNodes.begin());
            charNodeIterator != charNodes.end(); charNodeIterator++)
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
