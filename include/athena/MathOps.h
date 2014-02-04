#pragma once

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4.h"

static CVector3 operator *(float lhs, const CVector3& rhs)
{
	return CVector3(
		lhs * rhs.x, 
		lhs * rhs.y, 
		lhs * rhs.z);
}

static CVector3 operator *(const CVector3& lhs, const CMatrix4& rhs)
{
	CVector3 result;
	result.x = rhs(0, 0) * lhs.x + rhs(0, 1) * lhs.y + rhs(0, 2) * lhs.z;
	result.y = rhs(1, 0) * lhs.x + rhs(1, 1) * lhs.y + rhs(1, 2) * lhs.z;
	result.z = rhs(2, 0) * lhs.x + rhs(2, 1) * lhs.y + rhs(2, 2) * lhs.z;
	return result;
}
