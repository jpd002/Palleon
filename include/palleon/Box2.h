#pragma once

#include "math/Vector2.h"

class CBox2
{
public:
				CBox2();
				CBox2(const CVector2&, const CVector2&);
				CBox2(float, float, float, float);
	
	CVector2	GetEndPosition() const;

	bool		IsPointInBox(const CVector2&) const;
	bool		Intersects(const CBox2&) const;

	CBox2		Grow(const CVector2&) const;
	
	CVector2	position;
	CVector2	size;
};
