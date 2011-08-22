#ifndef _FRUSTUM_H_
#define _FRUSTUM_H_

#include "Plane.h"
#include "Vector3.h"

class CFrustum
{
public:
	bool		Intersects(const CVector3&, const CVector3&) const;

	CPlane		planes[6];
};

#endif
