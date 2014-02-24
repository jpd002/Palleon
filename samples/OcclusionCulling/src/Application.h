#pragma once

#include "AthenaEngine.h"
#include "TouchFreeCamera.h"
#include "OcclusionBuffer.h"
#include <deque>

namespace OcclusionCulling
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
		typedef std::vector<Athena::MeshPtr> MeshArray;

		void					CreateScene();
		void					CreateUi();
		void					UpdateMetrics();

		COcclusionBuffer		m_occlusionBuffer;

		Athena::PackagePtr		m_globalPackage;

		Athena::ViewportPtr		m_mainViewport;
		TouchFreeCameraPtr		m_mainCamera;

		MeshArray				m_occluders;
		MeshArray				m_occludees;

		Athena::ViewportPtr		m_uiViewport;

		Athena::LabelPtr		m_timeMetricLabel;
		Athena::LabelPtr		m_passCountMetricLabel;
		CBox2					m_forwardButtonBoundingBox;
		CBox2					m_backwardButtonBoundingBox;

		CVector2				m_mousePosition;
		float					m_updateTimeMetricTimer = 0;
		std::deque<double>		m_renderTimes;
	};
}
