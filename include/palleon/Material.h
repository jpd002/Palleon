#pragma once

#include <memory>
#include "palleon/resources/ResourceManager.h"
#include "palleon/Color.h"
#include "palleon/Matrix4.h"
#include "palleon/EffectParameterBag.h"

namespace Palleon
{
	class CMaterial;
	typedef std::shared_ptr<CMaterial> MaterialPtr;

	enum CULLING_MODE
	{
		CULLING_NONE,
		CULLING_CCW,
		CULLING_CW,
		CULLING_MODE_MAX
	};

	enum ALPHA_BLENDING_MODE
	{
		ALPHA_BLENDING_NONE,
		ALPHA_BLENDING_LERP,
		ALPHA_BLENDING_ADD,
		ALPHA_BLENDING_MODE_MAX
	};

	enum TEXTURE_COORD_SOURCE
	{
		TEXTURE_COORD_UV0,
		TEXTURE_COORD_UV1,
		TEXTURE_COORD_CUBE_POS,
		TEXTURE_COORD_CUBE_REFLECT,
		TEXTURE_COORD_SOURCE_MAX
	};

	enum TEXTURE_COMBINE_MODE
	{
		TEXTURE_COMBINE_MODULATE,
		TEXTURE_COMBINE_LERP,
		TEXTURE_COMBINE_ADD,
		TEXTURE_COMBINE_MODE_MAX
	};

	enum TEXTURE_ADDRESS_MODE
	{
		TEXTURE_ADDRESS_CLAMP = 0,
		TEXTURE_ADDRESS_REPEAT = 1,
		TEXTURE_ADDRESS_MODE_MAX
	};

	enum STENCIL_FUNCTION
	{
		STENCIL_FUNCTION_NEVER,
		STENCIL_FUNCTION_ALWAYS,
		STENCIL_FUNCTION_EQUAL,
		STENCIL_FUNCTION_MAX
	};

	enum STENCIL_FAIL_ACTION
	{
		STENCIL_FAIL_ACTION_KEEP,
		STENCIL_FAIL_ACTION_WRITE,
		STENCIL_FAIL_ACTION_MAX
	};

	class CMaterial : public CEffectParameterBag
	{
	public:
		enum
		{
			MAX_TEXTURE_SLOTS = 8,
		};

									CMaterial();
		virtual						~CMaterial();

		static MaterialPtr			Create();

		bool						operator <(const CMaterial&) const;

		ALPHA_BLENDING_MODE			GetAlphaBlendingMode() const;
		void						SetAlphaBlendingMode(ALPHA_BLENDING_MODE);

		CULLING_MODE				GetCullingMode() const;
		void						SetCullingMode(CULLING_MODE);

		CColor						GetColor() const;
		void						SetColor(const CColor&);

		TexturePtr					GetTexture(unsigned int) const;
		void						SetTexture(unsigned int, const TexturePtr&);

		const CMatrix4&				GetTextureMatrix(unsigned int) const;
		void						SetTextureMatrix(unsigned int, const CMatrix4&);

		TEXTURE_COORD_SOURCE		GetTextureCoordSource(unsigned int) const;
		void						SetTextureCoordSource(unsigned int, TEXTURE_COORD_SOURCE);

		TEXTURE_COMBINE_MODE		GetTextureCombineMode(unsigned int) const;
		void						SetTextureCombineMode(unsigned int, TEXTURE_COMBINE_MODE);

		TEXTURE_ADDRESS_MODE		GetTextureAddressModeU(unsigned int) const;
		void						SetTextureAddressModeU(unsigned int, TEXTURE_ADDRESS_MODE);

		TEXTURE_ADDRESS_MODE		GetTextureAddressModeV(unsigned int) const;
		void						SetTextureAddressModeV(unsigned int, TEXTURE_ADDRESS_MODE);

		bool						GetShadowCasting() const;
		void						SetShadowCasting(bool);

		bool						GetShadowReceiving() const;
		void						SetShadowReceiving(bool);

		bool						GetStencilEnabled() const;
		void						SetStencilEnabled(bool);

		STENCIL_FUNCTION			GetStencilFunction() const;
		void						SetStencilFunction(STENCIL_FUNCTION);

		STENCIL_FAIL_ACTION			GetStencilFailAction() const;
		void						SetStencilFailAction(STENCIL_FAIL_ACTION);

		uint8						GetStencilValue() const;
		void						SetStencilValue(uint8);

	protected:
		struct TEXTURE_SLOT
		{
			TexturePtr				texture;
			CMatrix4				matrix;
			TEXTURE_COORD_SOURCE	coordSource;
			TEXTURE_COMBINE_MODE	combineMode;
			TEXTURE_ADDRESS_MODE	addressModeU;
			TEXTURE_ADDRESS_MODE	addressModeV;
		};

		ALPHA_BLENDING_MODE			m_alphaBlendingMode;
		CULLING_MODE				m_cullingMode;
		CColor						m_color;

		bool						m_shadowCasting;
		bool						m_shadowReceiving;

		bool						m_stencilEnabled;
		STENCIL_FUNCTION			m_stencilFunction;
		STENCIL_FAIL_ACTION			m_stencilFailAction;
		uint8						m_stencilValue;

		TEXTURE_SLOT				m_textureSlots[MAX_TEXTURE_SLOTS];
	};
};
