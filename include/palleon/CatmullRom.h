#pragma once

#include "math/Vector2.h"

class CCatmullRom
{
public:
	static CVector2 Interpolate(float t, const CVector2& p0, const CVector2& p1, const CVector2& p2, const CVector2& p3);
	static CVector2 Derivative(float t, const CVector2& p0, const CVector2& p1, const CVector2& p2, const CVector2& p3);
};
