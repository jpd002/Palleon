#pragma once

#include <memory>
#include "SceneNode.h"
#include "Camera.h"
#include "EffectParameterBag.h"

namespace Palleon
{
	class CViewport;
	typedef std::shared_ptr<CViewport> ViewportPtr;

	class CViewport : public CEffectParameterBag
	{
	public:
								CViewport();
		virtual					~CViewport();

		static ViewportPtr		Create();

		CameraPtr				GetCamera() const;
		void					SetCamera(const CameraPtr&);

		CameraPtr				GetShadowCamera() const;
		void					SetShadowCamera(const CameraPtr&);

		SceneNodePtr			GetSceneRoot() const;

	protected:
		SceneNodePtr			m_sceneRoot;

		CameraPtr				m_camera;
		CameraPtr				m_shadowCamera;
	};
}
