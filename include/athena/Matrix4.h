#ifndef _MATRIX4_H_
#define _MATRIX4_H_

#include <memory.h>
#include <math.h>
#include <assert.h>

class CMatrix4
{
public:
	CMatrix4()
	{

	}
	
	void Clear()
	{
		memset(coeff, 0, sizeof(coeff));
	}

	float operator()(int row, int col) const
	{
		assert((row < 4) && (col < 4));
		return coeff[(row * 4) + col];
	}

	float& operator()(int row, int col)
	{
		assert((row < 4) && (col < 4));
		return coeff[(row * 4) + col];
	}
	
	CMatrix4 operator *(const CMatrix4& rhs) const
	{
		CMatrix4 result;
		for(unsigned int i = 0; i < 4; i++)
		{
			result(i, 0) = (*this)(i, 0) * rhs(0, 0) + (*this)(i, 1) * rhs(1, 0) + (*this)(i, 2) * rhs(2, 0) + (*this)(i, 3) * rhs(3, 0);
			result(i, 1) = (*this)(i, 0) * rhs(0, 1) + (*this)(i, 1) * rhs(1, 1) + (*this)(i, 2) * rhs(2, 1) + (*this)(i, 3) * rhs(3, 1);
			result(i, 2) = (*this)(i, 0) * rhs(0, 2) + (*this)(i, 1) * rhs(1, 2) + (*this)(i, 2) * rhs(2, 2) + (*this)(i, 3) * rhs(3, 2);
			result(i, 3) = (*this)(i, 0) * rhs(0, 3) + (*this)(i, 1) * rhs(1, 3) + (*this)(i, 2) * rhs(2, 3) + (*this)(i, 3) * rhs(3, 3);
		}
		return result;
	}

	static CMatrix4 MakeIdentity()
	{
		CMatrix4 result;
		result.Clear();
		result(0, 0) = 1;
		result(1, 1) = 1;
		result(2, 2) = 1;
		result(3, 3) = 1;
		return result;
	}

	static CMatrix4 MakeAxisXRotation(float angle)
	{
		CMatrix4 result;
		result.Clear();
		result(0, 0) = 1;
		result(1, 1) = cos(angle);
		result(1, 2) = -sin(angle);
		result(2, 1) = sin(angle);
		result(2, 2) = cos(angle);
		result(3, 3) = 1;
		return result;
	}

	static CMatrix4 MakeAxisYRotation(float angle)
	{
		CMatrix4 result;
		result.Clear();
		result(0, 0) = cos(angle);
		result(0, 2) = sin(angle);
		result(1, 1) = 1;
		result(2, 0) = -sin(angle);
		result(2, 2) = cos(angle);
		result(3, 3) = 1;
		return result;
	}

	static CMatrix4 MakeAxisZRotation(float angle)
	{
		CMatrix4 result;
		result.Clear();
		result(0, 0) = cos(angle);
		result(0, 1) = -sin(angle);
		result(1, 0) = sin(angle);
		result(1, 1) = cos(angle);
		result(2, 2) = 1;
		result(3, 3) = 1;
		return result;
	}

	static CMatrix4 MakeScale(float x, float y, float z)
	{
		CMatrix4 result;
		result.Clear();

		result(0, 0) = x;
		result(1, 1) = y;
		result(2, 2) = z;
		result(3, 3) = 1;

		return result;
	}

	static CMatrix4 MakeTranslation(float x, float y, float z)
	{
		CMatrix4 result;
		result.Clear();

		result(0, 0) = 1;
		result(1, 1) = 1;
		result(2, 2) = 1;
		result(3, 3) = 1;

		result(3, 0) = x;
		result(3, 1) = y;
		result(3, 2) = z;
		return result;
	}

	float coeff[16];
};

#endif
