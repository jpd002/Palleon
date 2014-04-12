#ifndef _PLANE_H_
#define _PLANE_H_

#include <math.h>

class CPlane
{
public:
	CPlane()
	{

	}

	CPlane(float a, float b, float c, float d)
		: a(a), b(b), c(c), d(d)
	{
	}

	CPlane Normalize() const
	{
		float length = sqrt((a * a) + (b * b) + (c * c));
		return CPlane(
			a / length,
			b / length,
			c / length,
			d / length);
	}

	float a;
	float b;
	float c;
	float d;
};

#endif
