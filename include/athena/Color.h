#ifndef _COLOR_H_
#define _COLOR_H_

#include "Types.h"

class CColor
{
public:
	CColor()
	{

	}

	CColor(float r, float g, float b, float a)
	: r(r)
	, g(g)
	, b(b)
	, a(a)
	{
		
	}
	
	CColor operator *(float value) const
	{
		return CColor(r * value, g * value, b * value, a * value);
	}

	float r;
	float g;
	float b;
	float a;
};

#endif
