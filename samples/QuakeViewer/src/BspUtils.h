#ifndef _BSPUTILS_H_
#define _BSPUTILS_H_

#include "BspDefs.h"
#include "athena/Vector3.h"

namespace Bsp
{
	static CVector3 ConvertToAthenaCoord(const float* coords)
	{
		return CVector3(coords[0], coords[2], coords[1]);
	}

	static CVector3 ConvertToAthenaCoord(float x, float y, float z)
	{
		return CVector3(x, z, y);
	}

	static CVector3 GetVertexPosition(const VERTEX& vertex)
	{
		return ConvertToAthenaCoord(vertex.position);
	}
};

#endif
