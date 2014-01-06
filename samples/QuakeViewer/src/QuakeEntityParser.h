#ifndef _QUAKEENTITYPARSER_H_
#define _QUAKEENTITYPARSER_H_

#include "QuakeEntity.h"

class CQuakeEntityParser
{
public:
	static QuakeEntityList	Parse(const char*);

private:
	enum PARSE_STATE
	{
		PARSE_STATE_OUTSIDE,
		PARSE_STATE_INSIDE_ENTITY,
		PARSE_STATE_INSIDE_KEY,
		PARSE_STATE_INSIDE_ENTITY_HAS_KEY,
		PARSE_STATE_INSIDE_VALUE,
	};

							CQuakeEntityParser();

	QuakeEntityList			ParseInternal(const char*);

	PARSE_STATE				m_parseState;
};

#endif
