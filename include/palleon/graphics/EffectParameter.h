#pragma once

#include "math/Vector2.h"
#include "math/Vector3.h"
#include "math/Vector4.h"

namespace Palleon
{
	enum EFFECT_PARAMETER_TYPE
	{
		EFFECT_PARAMETER_NULL,
		EFFECT_PARAMETER_SCALAR,
		EFFECT_PARAMETER_VECTOR2,
		EFFECT_PARAMETER_VECTOR3,
		EFFECT_PARAMETER_VECTOR4
	};

	class CEffectParameter
	{
	public:
									CEffectParameter();
		explicit					CEffectParameter(float);
		explicit					CEffectParameter(const CVector2&);
		explicit					CEffectParameter(const CVector3&);
		explicit					CEffectParameter(const CVector4&);

		bool						IsNull() const;
		void						SetNull();

		bool						IsScalar() const;
		float						GetScalar() const;
		void						SetScalar(float);

		bool						IsVector2() const;
		CVector2					GetVector2() const;
		void						SetVector2(const CVector2&);

		bool						IsVector3() const;
		CVector3					GetVector3() const;
		void						SetVector3(const CVector3&);

		bool						IsVector4() const;
		CVector4					GetVector4() const;
		void						SetVector4(const CVector4&);

	private:
		EFFECT_PARAMETER_TYPE		m_type = EFFECT_PARAMETER_NULL;

		CVector4					m_value = CVector4(0, 0, 0, 0);
	};
};
