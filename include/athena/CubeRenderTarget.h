#pragma once

#include "athena/Texture.h"
#include "athena/Viewport.h"

namespace Athena
{
	class CCubeRenderTarget : public CTexture
	{
	public:
		virtual			~CCubeRenderTarget();

		virtual void	Draw(TEXTURE_CUBE_FACE, const ViewportPtr&) = 0;
	};

	typedef std::shared_ptr<CCubeRenderTarget> CubeRenderTargetPtr;
}
