#pragma once

#include "PalleonEngine.h"
#include "TouchFreeCamera.h"
#include "OcclusionBuffer.h"
#include <deque>

namespace OcclusionCulling
{
	class CApplication : public Palleon::CApplication
	{
	public:
								CApplication();
		virtual					~CApplication();

		virtual void			Update(float) override;

		virtual void			NotifyMouseMove(int, int) override;
		virtual void			NotifyMouseDown() override;
		virtual void			NotifyMouseUp() override;

	private:
		typedef std::vector<Palleon::MeshPtr> MeshArray;

		void					CreateScene();
		void					CreateUi();
		void					UpdateMetrics();

		COcclusionBuffer		m_occlusionBuffer;

		Palleon::PackagePtr		m_globalPackage;

		Palleon::ViewportPtr	m_mainViewport;
		TouchFreeCameraPtr		m_mainCamera;

		MeshArray				m_occluders;
		MeshArray				m_occludees;

		Palleon::ViewportPtr	m_uiViewport;

		Palleon::LabelPtr		m_timeMetricLabel;
		Palleon::LabelPtr		m_passCountMetricLabel;
		CBox2					m_forwardButtonBoundingBox;
		CBox2					m_backwardButtonBoundingBox;

		CVector2				m_mousePosition;
		float					m_updateTimeMetricTimer = 0;
		std::deque<double>		m_renderTimes;
	};
}
