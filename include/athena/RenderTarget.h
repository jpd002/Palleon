#ifndef _RENDERTARGET_H_
#define _RENDERTARGET_H_

#include "athena/Texture.h"
#include "athena/Viewport.h"

namespace Athena
{
	class CRenderTarget : public CTexture
	{
	public:
		virtual			~CRenderTarget();

		virtual void	Draw(const ViewportPtr&) = 0;
	};

	typedef std::shared_ptr<CRenderTarget> RenderTargetPtr;
}

#endif
