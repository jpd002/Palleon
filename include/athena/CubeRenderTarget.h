#ifndef _CUBERENDERTARGET_H_
#define _CUBERENDERTARGET_H_

#include "athena/Texture.h"
#include "athena/Viewport.h"

namespace Athena
{
	enum CUBEMAP_FACE
	{
		CUBEMAP_FACE_POSITIVE_X,
		CUBEMAP_FACE_NEGATIVE_X,
		CUBEMAP_FACE_POSITIVE_Y,
		CUBEMAP_FACE_NEGATIVE_Y,
		CUBEMAP_FACE_POSITIVE_Z,
		CUBEMAP_FACE_NEGATIVE_Z,
		CUBEMAP_FACE_MAX
	};

	class CCubeRenderTarget : public CTexture
	{
	public:
		virtual			~CCubeRenderTarget();

		virtual void	Draw(CUBEMAP_FACE, const ViewportPtr&) = 0;
	};

	typedef std::shared_ptr<CCubeRenderTarget> CubeRenderTargetPtr;
}

#endif
