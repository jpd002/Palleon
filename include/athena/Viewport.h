#pragma once

#include <memory>
#include "SceneNode.h"
#include "Camera.h"

namespace Athena
{
	class CViewport;
	typedef std::shared_ptr<CViewport> ViewportPtr;

	class CViewport
	{
	public:
								CViewport();
		virtual					~CViewport();

		static ViewportPtr		Create();

		CameraPtr				GetCamera() const;
		void					SetCamera(const CameraPtr&);

		SceneNodePtr			GetSceneRoot() const;

	protected:
		SceneNodePtr			m_sceneRoot;

		CameraPtr				m_camera;
	};
}
