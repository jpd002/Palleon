#pragma once

#include <memory>
#include "palleon/Matrix4.h"
#include "palleon/graphics/Material.h"

namespace Palleon
{
	class CViewport;

	struct VIEWPORT_PARAMS
	{
		CViewport*	viewport = nullptr;
		CMatrix4	viewMatrix;
		CMatrix4	projMatrix;
		bool		hasShadowMap = false;
		CMatrix4	shadowViewProjMatrix = CMatrix4::MakeIdentity();
	};

	class CEffect
	{
	public:
		virtual					~CEffect() {};

		virtual void			UpdateConstants(const VIEWPORT_PARAMS&, CMaterial*, const CMatrix4&) = 0;
	};

	typedef std::shared_ptr<CEffect> EffectPtr;
}
