#pragma once

#ifdef _WIN32
#include "palleon/win32/Dx11Effect.h"
namespace Palleon
{
	typedef Palleon::CDx11Effect SpecificEffect;
}
#elif defined(__ANDROID__) or defined(__APPLE__)
#include "palleon/gles/GlEsEffect.h"
namespace Palleon
{
	typedef Palleon::CGlEsEffect SpecificEffect;
}
#endif

#include "ShaderBuilder.h"

namespace Palleon
{
	class CPlatformEffect : public SpecificEffect
	{
	public:
								CPlatformEffect(const CShaderBuilder&, const CShaderBuilder&);
		virtual					~CPlatformEffect();

		void					UpdateConstants(const VIEWPORT_PARAMS&, CMaterial*, const CMatrix4&) override;

		static const char*		g_worldMatrixName;
		static const char*		g_viewProjMatrixName;
		static const char*		g_worldViewProjMatrixName;
		static const char*		g_texture2MatrixName;
		static const char*		g_texture3MatrixName;

	protected:
		void					BeginConstantsUpdate();
		void					EndConstantsUpdate();
		virtual void			UpdateConstantsInner(const VIEWPORT_PARAMS&, CMaterial*, const CMatrix4&);
		void					SetConstant(const std::string&, const CMatrix4&);
		void					SetConstant(const std::string&, const CEffectParameter&);

	private:
#ifdef _WIN32
		typedef std::map<std::string, uint32> UniformOffsetMap;

		D3D11InputLayoutPtr		CreateInputLayout(const Palleon::VERTEX_BUFFER_DESCRIPTOR&) override;

		UniformOffsetMap		m_vertexUniformOffsets;
		uint8*					m_constantBufferPtr = nullptr;
#else
		typedef std::map<std::string, GLuint> UniformLocationMap;

		UniformLocationMap		m_vertexUniformLocations;
		GLuint					m_sampler0Location = -1;
		GLuint					m_sampler1Location = -1;
		GLuint					m_sampler2Location = -1;
		GLuint					m_sampler3Location = -1;
#endif
	};
}
