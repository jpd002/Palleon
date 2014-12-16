#pragma once

#include "math/Plane.h"
#include "math/Sphere.h"
#include "math/Vector3.h"

class CFrustum
{
public:
	bool		Intersects(const CSphere&) const;
	bool		Intersects(const CVector3&, const CVector3&) const;

	CPlane		planes[6];
};
