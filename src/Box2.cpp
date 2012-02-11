#include "athena/Box2.h"

CBox2::CBox2()
: position(0, 0)
, size(0, 0)
{
	
}

CBox2::CBox2(const CVector2& position, const CVector2& size)
: position(position)
, size(size)
{
	
}

CBox2::CBox2(float x, float y, float sx, float sy)
: position(CVector2(x, y))
, size(CVector2(sx, sy))
{
	
}

CVector2 CBox2::GetEndPosition() const
{
	return position + size;
}

bool CBox2::IsPointInBox(const CVector2& point) const
{
	if(point.x < position.x) return false;
	if(point.y < position.y) return false;
	if(point.x > position.x + size.x) return false;
	if(point.y > position.y + size.y) return false;
	return true;
}

bool CBox2::Intersects(const CBox2& rhs) const
{
	float minX0 = position.x;
	float maxX0 = position.x + size.x;
	
	float minX1 = rhs.position.x;
	float maxX1 = rhs.position.x + rhs.size.x;
	
	if(maxX0 < minX1) return false;
	if(minX0 > maxX1) return false;
	
	float minY0 = position.y;
	float maxY0 = position.y + size.y;
	
	float minY1 = rhs.position.y;
	float maxY1 = rhs.position.y + rhs.size.y;
	
	if(maxY0 < minY1) return false;
	if(minY0 > maxY1) return false;
	
	return true;
}

CBox2 CBox2::Grow(const CVector2& growAmount) const
{
	CBox2 result(*this);
	result.position -= growAmount / 2;
	result.size += growAmount;
	return result;
}
