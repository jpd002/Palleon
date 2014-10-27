#pragma once

#include "PalleonEngine.h"
#include "TouchFreeCamera.h"

namespace GodRays
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
		void						CreatePostProcess();
		void						CreateUi();

		void						RefreshUiLayout();

		//Resources
		Palleon::PackagePtr			m_globalPackage;

		//Post Process Viewport
		Palleon::ViewportPtr		m_postProcessViewport;
		Palleon::MeshPtr			m_postProcessMesh;
		
		//3D scene
		Palleon::ViewportPtr		m_sceneViewport;
		TouchFreeCameraPtr			m_sceneCamera;
		Palleon::SceneNodePtr		m_lightNode;

		Palleon::RenderTargetPtr	m_sceneRenderTarget;
		Palleon::RenderTargetPtr	m_unblurredRenderTarget;
		Palleon::RenderTargetPtr	m_blurredRenderTarget;
		Palleon::RenderTargetPtr	m_finalRenderTarget;

		//UI stuff
		Palleon::ViewportPtr		m_uiViewport;
		Palleon::ScenePtr			m_uiScene;

		CBox2						m_forwardButtonBoundingBox;
		CBox2						m_backwardButtonBoundingBox;

		CVector2					m_mousePosition = CVector2(0, 0);
		float						m_elapsed = 0;
		uint32						m_elapsedInt = 0;

		Palleon::EffectProviderPtr	m_occlusionMapEffectProvider;
		Palleon::EffectProviderPtr	m_godRayBlurEffectProvider;
		Palleon::EffectProviderPtr	m_godRayCompositeEffectProvider;
	};
}
