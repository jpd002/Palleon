#include <assert.h>
#include <boost/scoped_ptr.hpp>
#include "athena/NinePatchDescriptor.h"
#include "xml/Node.h"
#include "xml/Parser.h"
#include "xml/Utils.h"
#include "StdStream.h"

using namespace Athena;

CNinePatchDescriptor::CNinePatchDescriptor()
: m_width(0)
, m_height(0)
, m_leftMargin(0)
, m_topMargin(0)
, m_rightMargin(0)
, m_bottomMargin(0)
{

}

CNinePatchDescriptor::~CNinePatchDescriptor()
{

}

const char* CNinePatchDescriptor::GetTextureName() const
{
	return m_textureName.c_str();
}

unsigned int CNinePatchDescriptor::GetWidth() const
{
	return m_width;
}

unsigned int CNinePatchDescriptor::GetHeight() const
{
	return m_height;
}

unsigned int CNinePatchDescriptor::GetLeftMargin() const
{
	return m_leftMargin;
}

unsigned int CNinePatchDescriptor::GetTopMargin() const
{
	return m_topMargin;
}

unsigned int CNinePatchDescriptor::GetRightMargin() const
{
	return m_rightMargin;
}

unsigned int CNinePatchDescriptor::GetBottomMargin() const
{
	return m_bottomMargin;
}

void CNinePatchDescriptor::Load(const char* path)
{
	Framework::CStdStream inputStream(path, "rb");
	boost::scoped_ptr<Framework::Xml::CNode> document(Framework::Xml::CParser::ParseDocument(inputStream));

	auto ninePatchNode = document->Select("NinePatch");

	m_textureName = Framework::Xml::GetAttributeStringValue(ninePatchNode, "Texture");
	m_width = Framework::Xml::GetAttributeIntValue(ninePatchNode, "Width");
	m_height = Framework::Xml::GetAttributeIntValue(ninePatchNode, "Height");
	m_leftMargin = Framework::Xml::GetAttributeIntValue(ninePatchNode, "LeftMargin");
	m_topMargin = Framework::Xml::GetAttributeIntValue(ninePatchNode, "TopMargin");
	m_rightMargin = Framework::Xml::GetAttributeIntValue(ninePatchNode, "RightMargin");
	m_bottomMargin = Framework::Xml::GetAttributeIntValue(ninePatchNode, "BottomMargin");
}
