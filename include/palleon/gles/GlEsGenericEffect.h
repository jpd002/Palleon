#pragma once

#include <map>
#include "GlEsEffect.h"
#include "../graphics/GenericEffect.h"
#include "../graphics/ShaderBuilder.h"

namespace Palleon
{
	class CGlEsGenericEffect : public CGlEsEffect, public CGenericEffect
	{
	public:
								CGlEsGenericEffect(const EffectInputBindingArray&, const CShaderBuilder&, const CShaderBuilder&);
		virtual					~CGlEsGenericEffect();

	protected:
		void					UpdateConstants(const VIEWPORT_PARAMS&, CMaterial*, const CMatrix4&) override;
		void					SetConstant(const std::string&, const CMatrix4&) override;
		void					SetConstant(const std::string&, const CEffectParameter&) override;

	private:
		typedef std::map<std::string, GLuint> UniformLocationMap;

		UniformLocationMap		m_uniformLocations;
		GLuint					m_sampler0Location = -1;
		GLuint					m_sampler1Location = -1;
		GLuint					m_sampler2Location = -1;
		GLuint					m_sampler3Location = -1;
	};

	typedef CGlEsGenericEffect CPlatformGenericEffect;
}
