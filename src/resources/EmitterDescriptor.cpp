#include "palleon/resources/EmitterDescriptor.h"
#include "xml/Parser.h"
#include "xml/Utils.h"
#include <memory>

using namespace Palleon;

CEmitterDescriptor::CEmitterDescriptor()
: m_sourceType(EMITTER_SOURCE_POINT)
, m_initialVelocityRange(0.0f, 0.0f)
, m_initialLifeRange(0.0f, 0.0f)
, m_initialSizeRange(CVector2(1.0f, 1.0f), CVector2(1.0f, 1.0f))
, m_lineStart(0, 0)
, m_lineEnd(0, 0)
{

}

CEmitterDescriptor::~CEmitterDescriptor()
{

}

void CEmitterDescriptor::Load(Framework::CStream& inputStream)
{
	std::unique_ptr<Framework::Xml::CNode> document(Framework::Xml::CParser::ParseDocument(inputStream));

	auto emitterNode = document->Select("Emitter");

	const char* sourceType(nullptr);
	if(Framework::Xml::GetAttributeStringValue(emitterNode, "Source", &sourceType))
	{
		if(!strcmp(sourceType, "Line"))			m_sourceType = EMITTER_SOURCE_LINE;
		else if(!strcmp(sourceType, "Point"))	m_sourceType = EMITTER_SOURCE_POINT;
	}

	Framework::Xml::GetAttributeFloatValue(emitterNode, "LineStartX", &m_lineStart.x);
	Framework::Xml::GetAttributeFloatValue(emitterNode, "LineStartY", &m_lineStart.y);
	Framework::Xml::GetAttributeFloatValue(emitterNode, "LineEndX", &m_lineEnd.x);
	Framework::Xml::GetAttributeFloatValue(emitterNode, "LineEndY", &m_lineEnd.y);

	m_initialVelocityRange.first	= Framework::Xml::GetAttributeFloatValue(emitterNode, "MinInitVelocity");
	m_initialVelocityRange.second	= Framework::Xml::GetAttributeFloatValue(emitterNode, "MaxInitVelocity");
	m_initialLifeRange.first		= Framework::Xml::GetAttributeFloatValue(emitterNode, "MinInitLife");
	m_initialLifeRange.second		= Framework::Xml::GetAttributeFloatValue(emitterNode, "MaxInitLife");
	m_initialSizeRange.first.x		= Framework::Xml::GetAttributeFloatValue(emitterNode, "MinInitSizeX");
	m_initialSizeRange.first.y		= Framework::Xml::GetAttributeFloatValue(emitterNode, "MinInitSizeY");
	m_initialSizeRange.second.x		= Framework::Xml::GetAttributeFloatValue(emitterNode, "MaxInitSizeX");
	m_initialSizeRange.second.y		= Framework::Xml::GetAttributeFloatValue(emitterNode, "MaxInitSizeY");
}

EMITTER_SOURCE_TYPE CEmitterDescriptor::GetSourceType() const
{
	return m_sourceType;
}

CVector2 CEmitterDescriptor::GetLineSourceStart() const
{
	return m_lineStart;
}

CVector2 CEmitterDescriptor::GetLineSourceEnd() const
{
	return m_lineEnd;
}

FloatRange CEmitterDescriptor::GetInitialVelocityRange() const
{
	return m_initialVelocityRange;
}

FloatRange CEmitterDescriptor::GetInitialLifeRange() const
{
	return m_initialLifeRange;
}

Vector2Range CEmitterDescriptor::GetInitialSizeRange() const
{
	return m_initialSizeRange;
}
