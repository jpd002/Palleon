#ifndef _QUAKEENTITY_H_
#define _QUAKEENTITY_H_

#include <list>
#include <unordered_map>
#include "PalleonEngine.h"

struct QUAKE_ENTITY
{
	typedef std::unordered_map<std::string, std::string> ValueMap;

	ValueMap		values;
};

typedef std::list<QUAKE_ENTITY> QuakeEntityList;

namespace QuakeEntity
{
	const QUAKE_ENTITY*		FindFirstEntityWithClassName(const QuakeEntityList&, const char*);
	CVector3				ParseEntityVector(const char*);
}

#endif
