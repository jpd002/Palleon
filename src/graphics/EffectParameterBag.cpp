#include <assert.h>
#include "palleon/graphics/EffectParameterBag.h"

using namespace Palleon;

CEffectParameter CEffectParameterBag::GetEffectParameter(const std::string& name) const
{
	auto paramIterator = m_effectParameters.find(name);
	if(paramIterator != std::end(m_effectParameters))
	{
		return paramIterator->second;
	}
	else
	{
		return CEffectParameter();
	}
}

void CEffectParameterBag::SetEffectParameter(const std::string& name, const CEffectParameter& param)
{
	m_effectParameters[name] = param;
}
