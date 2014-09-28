#pragma once

#include "palleon/Texture.h"
#include "palleon/Viewport.h"

namespace Palleon
{
	class CRenderTarget : public CTexture
	{
	public:
		virtual			~CRenderTarget();

		virtual void	Draw(const ViewportPtr&) = 0;
	};

	typedef std::shared_ptr<CRenderTarget> RenderTargetPtr;
}
