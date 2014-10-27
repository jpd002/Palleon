#pragma once

#include "palleon/graphics/Material.h"
#include "palleon/graphics/Effect.h"
#include "palleon/graphics/ShaderBuilder.h"
#include "palleon/graphics/VertexBuffer.h"

namespace Palleon
{
	struct EFFECTINPUTBINDING
	{
		EFFECTINPUTBINDING(VERTEX_ITEM_ID vertexItemId, SEMANTIC semantic, unsigned int semanticIndex)
			: vertexItemId(vertexItemId), semantic(semantic), semanticIndex(semanticIndex)
		{
				
		}

		VERTEX_ITEM_ID		vertexItemId = VERTEX_ITEM_ID_NONE;
		SEMANTIC			semantic = SEMANTIC_NONE;
		unsigned int		semanticIndex = 0;
	};

	typedef std::vector<EFFECTINPUTBINDING> EffectInputBindingArray;

	class CGenericEffect
	{
	public:
		
	protected:
		static const char*		g_worldMatrixName;
		static const char*		g_viewProjMatrixName;
		static const char*		g_worldViewProjMatrixName;
		static const char*		g_texture2MatrixName;
		static const char*		g_texture3MatrixName;

		static bool				IsVertexShaderUniform(const char*);
		static bool				IsPixelShaderUniform(const char*);

		virtual void			UpdateSpecificConstants(const VIEWPORT_PARAMS&, CMaterial*, const CMatrix4&) {}
		virtual void			SetConstant(const std::string&, const CMatrix4&) = 0;
		virtual void			SetConstant(const std::string&, const CEffectParameter&) = 0;

		void					UpdateGenericConstants(const VIEWPORT_PARAMS&, CMaterial*, const CMatrix4&);
	};
}
