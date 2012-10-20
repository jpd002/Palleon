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
	
	CColor operator +(const CColor& rhs) const
	{
		return CColor(
						r + rhs.r,
						g + rhs.g,
						b + rhs.b,
						a + rhs.a);
	}

	CColor operator -(const CColor& rhs) const
	{
		return CColor(
						r - rhs.r,
						g - rhs.g,
						b - rhs.b,
						a - rhs.a);
	}

	CColor operator *(float value) const
	{
		return CColor(r * value, g * value, b * value, a * value);
	}

	CColor operator /(float value) const
	{
		return CColor(r / value, g / value, b / value, a / value);
	}

	bool operator ==(const CColor& rhs) const
	{
		return (r == rhs.r) && (g == rhs.g) && (b == rhs.b) && (a == rhs.a);
	}

	CColor operator -() const
	{
		return CColor(-r, -g, -b, -a);
	}

	CColor MultiplyClamp(float value) const
	{
		CColor result;
		result.r = std::min<float>(std::max<float>(r * value, 0.0f), 1.0f);
		result.g = std::min<float>(std::max<float>(g * value, 0.0f), 1.0f);
		result.b = std::min<float>(std::max<float>(b * value, 0.0f), 1.0f);
		result.a = std::min<float>(std::max<float>(a * value, 0.0f), 1.0f);
		return result;
	}

	float r;
	float g;
	float b;
	float a;
};

static CColor operator *(float lhs, const CColor& rhs)
{
	return CColor(
		lhs * rhs.r, 
		lhs * rhs.g, 
		lhs * rhs.b,
		lhs * rhs.a);
}

#endif
