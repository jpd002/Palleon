#ifndef _VIEWPORT_H_
#define _VIEWPORT_H_

#include <memory>
#include "SceneNode.h"
#include "Camera.h"

namespace Athena
{
	class CViewport;
	typedef std::tr1::shared_ptr<CViewport> ViewportPtr;

	class CViewport
	{
	public:
		virtual					~CViewport();

		static ViewportPtr		Create();

		CameraPtr				GetCamera() const;
		void					SetCamera(const CameraPtr&);

		SceneNodePtr			GetSceneRoot() const;

	protected:
								CViewport();

		SceneNodePtr			m_sceneRoot;

		CameraPtr				m_camera;
	};
}

#endif
