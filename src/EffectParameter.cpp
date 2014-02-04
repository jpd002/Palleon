#include <assert.h>
#include "athena/EffectParameter.h"

using namespace Athena;

bool CEffectParameter::IsNull() const
{
	return m_type == EFFECT_PARAMETER_NULL;
}

void CEffectParameter::SetNull()
{
	m_type = EFFECT_PARAMETER_NULL;
}

bool CEffectParameter::IsScalar() const
{
	return m_type == EFFECT_PARAMETER_SCALAR;
}

float CEffectParameter::GetScalar() const
{
	assert(m_type == EFFECT_PARAMETER_SCALAR);
	return m_value.x;
}

void CEffectParameter::SetScalar(float value)
{
	m_type = EFFECT_PARAMETER_SCALAR;
	m_value.x = value;
}

bool CEffectParameter::IsVector2() const
{
	return m_type == EFFECT_PARAMETER_VECTOR2;
}

CVector2 CEffectParameter::GetVector2() const
{
	assert(m_type == EFFECT_PARAMETER_VECTOR2);
	return CVector2(m_value.x, m_value.y);
}

void CEffectParameter::SetVector2(const CVector2& value)
{
	m_type = EFFECT_PARAMETER_VECTOR2;
	m_value.x = value.x;
	m_value.y = value.y;
	m_value.z = 0;
	m_value.w = 0;
}

bool CEffectParameter::IsVector3() const
{
	return m_type == EFFECT_PARAMETER_VECTOR3;
}

CVector3 CEffectParameter::GetVector3() const
{
	assert(m_type == EFFECT_PARAMETER_VECTOR3);
	return CVector3(m_value.x, m_value.y, m_value.z);
}

void CEffectParameter::SetVector3(const CVector3& value)
{
	m_type = EFFECT_PARAMETER_VECTOR3;
	m_value.x = value.x;
	m_value.y = value.y;
	m_value.z = value.z;
	m_value.w = 0;
}

bool CEffectParameter::IsVector4() const
{
	return m_type == EFFECT_PARAMETER_VECTOR4;
}

CVector4 CEffectParameter::GetVector4() const
{
	assert(m_type == EFFECT_PARAMETER_VECTOR4);
	return m_value;
}

void CEffectParameter::SetVector4(const CVector4& value)
{
	m_type = EFFECT_PARAMETER_VECTOR4;
	m_value = value;
}
