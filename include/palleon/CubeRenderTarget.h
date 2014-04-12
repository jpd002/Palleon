#pragma once

#include "palleon/Texture.h"
#include "palleon/Viewport.h"

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
