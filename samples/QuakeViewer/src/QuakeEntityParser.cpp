#include "QuakeEntityParser.h"

CQuakeEntityParser::CQuakeEntityParser()
: m_parseState(PARSE_STATE_OUTSIDE)
{

}

QuakeEntityList CQuakeEntityParser::Parse(const char* entities)
{
	CQuakeEntityParser parser;
	return parser.ParseInternal(entities);
}

QuakeEntityList CQuakeEntityParser::ParseInternal(const char* entities)
{
	std::string currentKey, currentValue;
	currentKey.reserve(100);
	currentValue.reserve(100);
	QUAKE_ENTITY currentEntity;
	QuakeEntityList result;
	while(1)
	{
		char currentChar = (*entities++);
		if(currentChar == 0) break;

		switch(m_parseState)
		{
		case PARSE_STATE_OUTSIDE:
			if(currentChar == '{')
			{
				currentEntity.values.clear();
				m_parseState = PARSE_STATE_INSIDE_ENTITY;
			}
			break;
		case PARSE_STATE_INSIDE_ENTITY:
			if(currentChar == '\"')
			{
				currentKey = "";
				m_parseState = PARSE_STATE_INSIDE_KEY;
			}
			else if(currentChar == '}')
			{
				result.push_back(currentEntity);
				m_parseState = PARSE_STATE_OUTSIDE;
			}
			break;
		case PARSE_STATE_INSIDE_ENTITY_HAS_KEY:
			if(currentChar == '\"')
			{
				currentValue = "";
				m_parseState = PARSE_STATE_INSIDE_VALUE;
			}
			break;
		case PARSE_STATE_INSIDE_KEY:
			if(currentChar == '\"')
			{
				m_parseState = PARSE_STATE_INSIDE_ENTITY_HAS_KEY;
			}
			else
			{
				currentKey += currentChar;
			}
			break;
		case PARSE_STATE_INSIDE_VALUE:
			if(currentChar == '\"')
			{
				currentEntity.values.insert(QUAKE_ENTITY::ValueMap::value_type(currentKey, currentValue));
				m_parseState = PARSE_STATE_INSIDE_ENTITY;
			}
			else
			{
				currentValue += currentChar;
			}
			break;
		}
	}

	assert(m_parseState == PARSE_STATE_OUTSIDE);
	return result;
}
