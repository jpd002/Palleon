#pragma once

#include "AthenaEngine.h"
#include "TouchFreeCamera.h"

namespace ShadowMapping
{
	class CApplication : public Athena::CApplication
	{
	public:
								CApplication();
		virtual					~CApplication();

		virtual void			Update(float) override;

		virtual void			NotifyMouseMove(int, int) override;
		virtual void			NotifyMouseDown() override;
		virtual void			NotifyMouseUp() override;

	private:
		void					CreateScene();
		void					CreateUi();

		void					UpdateShadowCamera();

		Athena::PackagePtr		m_globalPackage;

		Athena::ViewportPtr		m_mainViewport;
		TouchFreeCameraPtr		m_mainCamera;
		Athena::CameraPtr		m_shadowCamera;
		Athena::MeshPtr			m_shadowCameraSphere;

		Athena::ViewportPtr		m_uiViewport;

		CBox2					m_forwardButtonBoundingBox;
		CBox2					m_backwardButtonBoundingBox;

		CVector2				m_mousePosition;
		float					m_elapsed;
	};
}
