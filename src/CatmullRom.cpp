#include "palleon/CatmullRom.h"

CVector2 CCatmullRom::Interpolate(float t, const CVector2& p0, const CVector2& p1, const CVector2& p2, const CVector2& p3)
{
	CVector2 result(0, 0);
	float s1 = t;
	float s2 = s1 * t;
	float s3 = s2 * t;
	
	result = 0.5f * (
					 (2 * p1) +
					 s1 * (-p0 + p2) + 
					 s2 * (2 * p0 - 5 * p1 + 4 * p2 - p3) +
					 s3 * (-p0 + 3 * p1 - 3 * p2 + p3)
					 );
	
	return result;
}

CVector2 CCatmullRom::Derivative(float t, const CVector2& p0, const CVector2& p1, const CVector2& p2, const CVector2& p3)
{
	CVector2 result(0, 0);
	float s1 = t;
	float s2 = s1 * t;
	
	result = 0.5f * (
					 (-p0 + p2) + 
					 2 * s1 * (2 * p0 - 5 * p1 + 4 * p2 - p3) +
					 3 * s2 * (-p0 + 3 * p1 - 3 * p2 + p3)
					 );
	
	return result;
}
