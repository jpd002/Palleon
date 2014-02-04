#include "athena/Frustum.h"

bool CFrustum::Intersects(const CSphere& sphere) const
{
	for(const auto& plane : planes)
	{
		CVector3 normal(plane.a, plane.b, plane.c);
		float distance = normal.Dot(sphere.position) + plane.d;
		if(distance < -sphere.radius)
		{
			return false;
		}
	}
	return true;
}

bool CFrustum::Intersects(const CVector3& boxMin, const CVector3& boxMax) const
{
	CVector3 corners[8];

	corners[0] = CVector3(boxMin.x, boxMin.y, boxMin.z);
	corners[1] = CVector3(boxMin.x, boxMin.y, boxMax.z);
	corners[2] = CVector3(boxMin.x, boxMax.y, boxMin.z);
	corners[3] = CVector3(boxMin.x, boxMax.y, boxMax.z);
	corners[4] = CVector3(boxMax.x, boxMin.y, boxMin.z);
	corners[5] = CVector3(boxMax.x, boxMin.y, boxMax.z);
	corners[6] = CVector3(boxMax.x, boxMax.y, boxMin.z);
	corners[7] = CVector3(boxMax.x, boxMax.y, boxMax.z);

	for(const auto& plane : planes)
	{
		unsigned int inCount = 8;

		CVector3 normal(plane.a, plane.b, plane.c);

		for(unsigned int i = 0; i < 8; i++)
		{
			float distance = normal.Dot(corners[i]) + plane.d;
			if(distance < 0)
			{
				inCount--;
			}
		}

		if(inCount == 0)
		{
			return false;
		}
	}

	return true;
}
