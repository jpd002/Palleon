#ifndef _DX9RENDERTARGET_H_
#define _DX9RENDERTARGET_H_

#include <d3d9.h>
#include "Types.h"
#include "athena/RenderTarget.h"

namespace Athena
{
	class CDx9RenderTarget : public CRenderTarget
	{
	public:
							CDx9RenderTarget(IDirect3DDevice9*, TEXTURE_FORMAT, uint32, uint32);
		virtual				~CDx9RenderTarget();

		void				Draw(const ViewportPtr&);

		virtual void*		GetHandle() const;

	private:
		IDirect3DTexture9*	m_texture;
	};
}

#endif
