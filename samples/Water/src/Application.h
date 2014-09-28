#pragma once

#include "PalleonEngine.h"
#include "TouchFreeCamera.h"

namespace Water
{
	class CApplication : public Palleon::CApplication
	{
	public:
									CApplication();
		virtual						~CApplication();

		virtual void				Update(float) override;

		virtual void				NotifySizeChanged() override;

		virtual void				NotifyMouseMove(int, int) override;
		virtual void				NotifyMouseDown() override;
		virtual void				NotifyMouseUp() override;

	private:
		void						CreateScene();
		void						CreateUi();

		void						RefreshUiLayout();

		//Resources
		Palleon::PackagePtr			m_globalPackage;

		//3D scene
		Palleon::ViewportPtr		m_mainViewport;
		TouchFreeCameraPtr			m_mainCamera;
		Palleon::CameraPtr			m_reflectCamera;
		Palleon::MeshPtr			m_waterPlane;
		Palleon::MeshPtr			m_overBall;
		Palleon::MeshPtr			m_underBall;
		Palleon::RenderTargetPtr	m_reflectRenderTarget;
		Palleon::RenderTargetPtr	m_refractRenderTarget;
		Palleon::EffectProviderPtr	m_waterEffectProvider;

		//UI stuff
		Palleon::ViewportPtr		m_uiViewport;
		Palleon::ScenePtr			m_uiScene;

		CBox2						m_forwardButtonBoundingBox;
		CBox2						m_backwardButtonBoundingBox;

		CVector2					m_mousePosition;
		float						m_elapsed;
	};
}
