#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include "EffectParameter.h"

namespace Palleon
{
	class CEffectParameterBag
	{
	public:
		CEffectParameter			GetEffectParameter(const std::string&) const;
		void						SetEffectParameter(const std::string&, const CEffectParameter&);

		template <typename ParamType>
		ParamType GetEffectParamOrDefault(const std::string& paramName, const ParamType& defaultValue, bool mustExist = false) const;

		template <>
		float GetEffectParamOrDefault<float>(const std::string& paramName, const float& defaultValue, bool mustExist) const
		{
			auto effectParam = GetEffectParameter(paramName);
			assert(!mustExist || !effectParam.IsNull());
			if(effectParam.IsNull()) return defaultValue;
			return effectParam.GetScalar();
		}

		template <>
		CVector2 GetEffectParamOrDefault<CVector2>(const std::string& paramName, const CVector2& defaultValue, bool mustExist) const
		{
			auto effectParam = GetEffectParameter(paramName);
			assert(!mustExist || !effectParam.IsNull());
			if(effectParam.IsNull()) return defaultValue;
			return effectParam.GetVector2();
		}

		template <>
		CVector3 GetEffectParamOrDefault<CVector3>(const std::string& paramName, const CVector3& defaultValue, bool mustExist) const
		{
			auto effectParam = GetEffectParameter(paramName);
			assert(!mustExist || !effectParam.IsNull());
			if(effectParam.IsNull()) return defaultValue;
			return effectParam.GetVector3();
		}

		template <>
		CVector4 GetEffectParamOrDefault<CVector4>(const std::string& paramName, const CVector4& defaultValue, bool mustExist) const
		{
			auto effectParam = GetEffectParameter(paramName);
			assert(!mustExist || !effectParam.IsNull());
			if(effectParam.IsNull()) return defaultValue;
			if(effectParam.IsVector3())
			{
				return CVector4(effectParam.GetVector3(), 0);
			}
			else
			{
				return effectParam.GetVector4();
			}
		}

	private:
		typedef std::unordered_map<std::string, CEffectParameter> EffectParameterMap;

		EffectParameterMap			m_effectParameters;
	};
};
