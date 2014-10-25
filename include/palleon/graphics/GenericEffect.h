#pragma once

#include "../Material.h"
#include "../Effect.h"

namespace Palleon
{
	class CGenericEffect
	{
	public:
		
	protected:
		static const char*		g_worldMatrixName;
		static const char*		g_viewProjMatrixName;
		static const char*		g_worldViewProjMatrixName;
		static const char*		g_texture2MatrixName;
		static const char*		g_texture3MatrixName;

		virtual void			UpdateSpecificConstants(const VIEWPORT_PARAMS&, CMaterial*, const CMatrix4&) = 0;
		virtual void			SetConstant(const std::string&, const CMatrix4&) = 0;
		virtual void			SetConstant(const std::string&, const CEffectParameter&) = 0;

		void					UpdateGenericConstants(const VIEWPORT_PARAMS&, CMaterial*, const CMatrix4&);
	};
}
