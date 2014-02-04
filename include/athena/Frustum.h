#pragma once

#include "Plane.h"
#include "Sphere.h"
#include "Vector3.h"

class CFrustum
{
public:
	bool		Intersects(const CSphere&) const;
	bool		Intersects(const CVector3&, const CVector3&) const;

	CPlane		planes[6];
};
