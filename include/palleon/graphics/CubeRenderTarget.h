#pragma once

#include "palleon/graphics/Texture.h"
#include "palleon/graphics/Viewport.h"

namespace Palleon
{
	class CCubeRenderTarget : public CTexture
	{
	public:
		virtual			~CCubeRenderTarget();

		virtual void	Draw(TEXTURE_CUBE_FACE, const ViewportPtr&) = 0;
	};

	typedef std::shared_ptr<CCubeRenderTarget> CubeRenderTargetPtr;
}
