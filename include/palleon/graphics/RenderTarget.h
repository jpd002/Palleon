#pragma once

#include "palleon/graphics/Texture.h"
#include "palleon/graphics/Viewport.h"

namespace Palleon
{
	class CRenderTarget : public CTexture
	{
	public:
		virtual			~CRenderTarget();

		virtual void	Clear() = 0;
		virtual void	Draw(const ViewportPtr&) = 0;
	};

	typedef std::shared_ptr<CRenderTarget> RenderTargetPtr;
}
