#ifndef _DX9CUBERENDERTARGET_H_
#define _DX9CUBERENDERTARGET_H_

#include <d3d9.h>
#include "Types.h"
#include "athena/CubeRenderTarget.h"

namespace Athena
{
	class CDx9CubeRenderTarget : public CCubeRenderTarget
	{
	public:
								CDx9CubeRenderTarget(IDirect3DDevice9*, TEXTURE_FORMAT, uint32);
		virtual					~CDx9CubeRenderTarget();

		void					Draw(CUBEMAP_FACE, const ViewportPtr&);

		virtual void*			GetHandle() const;

	private:
		IDirect3DCubeTexture9*	m_texture;
	};
}

#endif
