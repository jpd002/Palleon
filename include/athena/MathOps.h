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

static CVector4 operator *(const CVector4& lhs, const CMatrix4& rhs)
{
	CVector4 result;
	result.x = rhs(0, 0) * lhs.x + rhs(0, 1) * lhs.y + rhs(0, 2) * lhs.z + rhs(0, 3) * lhs.w;
	result.y = rhs(1, 0) * lhs.x + rhs(1, 1) * lhs.y + rhs(1, 2) * lhs.z + rhs(1, 3) * lhs.w;
	result.z = rhs(2, 0) * lhs.x + rhs(2, 1) * lhs.y + rhs(2, 2) * lhs.z + rhs(2, 3) * lhs.w;
	result.w = rhs(3, 0) * lhs.x + rhs(3, 1) * lhs.y + rhs(3, 2) * lhs.z + rhs(3, 3) * lhs.w;
	return result;
}

static CVector4 operator *(const CMatrix4& lhs, const CVector4& rhs)
{
	CVector4 result;
	result.x = lhs(0, 0) * rhs.x + lhs(1, 0) * rhs.y + lhs(2, 0) * rhs.z + lhs(3, 0) * rhs.w;
	result.y = lhs(0, 1) * rhs.x + lhs(1, 1) * rhs.y + lhs(2, 1) * rhs.z + lhs(3, 1) * rhs.w;
	result.z = lhs(0, 2) * rhs.x + lhs(1, 2) * rhs.y + lhs(2, 2) * rhs.z + lhs(3, 2) * rhs.w;
	result.w = lhs(0, 3) * rhs.x + lhs(1, 3) * rhs.y + lhs(2, 3) * rhs.z + lhs(3, 3) * rhs.w;
	return result;
}
