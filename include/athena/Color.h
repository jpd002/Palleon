#ifndef _COLOR_H_
#define _COLOR_H_

#include "Types.h"

class CColor
{
public:
	CColor()
	: r(0)
	, g(0)
	, b(0)
	, a(0)
	{
		
	}
	
	CColor(float r, float g, float b, float a)
	: r(r)
	, g(g)
	, b(b)
	, a(a)
	{
		
	}
	
	CColor(uint8 r, uint8 g, uint8 b, uint8 a)
	: r(static_cast<float>(r) / 255.f)
	, g(static_cast<float>(g) / 255.f)
	, b(static_cast<float>(b) / 255.f)
	, a(static_cast<float>(a) / 255.f)
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
