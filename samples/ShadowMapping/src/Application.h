#pragma once

#include "PalleonEngine.h"
#include "TouchFreeCamera.h"

namespace ShadowMapping
{
	class CApplication : public Palleon::CApplication
	{
	public:
								CApplication();
		virtual					~CApplication();

		virtual void			Update(float) override;

		virtual void			NotifySizeChanged() override;

		virtual void			NotifyMouseMove(int, int) override;
		virtual void			NotifyMouseDown() override;
		virtual void			NotifyMouseUp() override;

	private:
		void					CreateScene();
		void					CreateUi();

		void					UpdateShadowCamera();
		void					RefreshUiLayout();

		Palleon::PackagePtr		m_globalPackage;

		Palleon::ViewportPtr	m_mainViewport;
		TouchFreeCameraPtr		m_mainCamera;
		Palleon::CameraPtr		m_shadowCamera;
		Palleon::MeshPtr		m_shadowCameraSphere;

		Palleon::ViewportPtr	m_uiViewport;

		Palleon::ScenePtr		m_uiScene;

		CBox2					m_forwardButtonBoundingBox;
		CBox2					m_backwardButtonBoundingBox;

		CVector2				m_mousePosition;
		float					m_elapsed;
	};
}
