#pragma once

#include "Dx11Effect.h"
#include "../graphics/GenericEffect.h"
#include "../graphics/ShaderBuilder.h"

namespace Palleon
{
	class CDx11GenericEffect : public CDx11Effect, public CGenericEffect
	{
	public:
								CDx11GenericEffect(CShaderBuilder&, CShaderBuilder&);
		virtual					~CDx11GenericEffect();

	protected:
		void					UpdateConstants(const VIEWPORT_PARAMS&, CMaterial*, const CMatrix4&) override;
		void					SetConstant(const std::string&, const CMatrix4&) override;
		void					SetConstant(const std::string&, const CEffectParameter&) override;

	private:
		typedef std::map<std::string, uint32> UniformOffsetMap;

		void					BeginConstantsUpdate();
		void					EndConstantsUpdate();

		D3D11InputLayoutPtr		CreateInputLayout(const Palleon::VERTEX_BUFFER_DESCRIPTOR&) override;

		UniformOffsetMap		m_vertexUniformOffsets;
		uint8*					m_constantBufferPtr = nullptr;
	};

	typedef CDx11GenericEffect CPlatformGenericEffect;
}
