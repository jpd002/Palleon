#include "QuakeEntity.h"

const QUAKE_ENTITY* QuakeEntity::FindFirstEntityWithClassName(const QuakeEntityList& entities, const char* className)
{
	for(auto entityIterator(std::begin(entities)); entityIterator != std::end(entities); entityIterator++)
	{
		const QUAKE_ENTITY& entity(*entityIterator);
		auto classNameIterator = entity.values.find("classname");
		if(classNameIterator != std::end(entity.values))
		{
			if(classNameIterator->second == className) return &entity;
		}
	}

	return nullptr;
}

CVector3 QuakeEntity::ParseEntityVector(const char* vectorString)
{
	float posX = 0, posY = 0, posZ = 0;
	int parsed = sscanf(vectorString, "%f %f %f", &posX, &posY, &posZ);
	assert(parsed == 3);
	return CVector3(posX, posZ, posY);
}
