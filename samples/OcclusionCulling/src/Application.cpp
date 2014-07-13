#include "Application.h"
#include "OcclusionBuffer.h"
#include "bitmap/BMP.h"
#include "StdStreamUtils.h"
#include "string_format.h"
#include <Windows.h>

using namespace OcclusionCulling;

CApplication::CApplication()
: m_mousePosition(0, 0)
{
	m_renderTimes.resize(10);

	m_globalPackage = Palleon::CPackage::Create("global");

	CreateScene();
	CreateUi();

	Palleon::CGraphicDevice::GetInstance().AddViewport(m_mainViewport.get());
	Palleon::CGraphicDevice::GetInstance().AddViewport(m_uiViewport.get());
}

CApplication::~CApplication()
{
	Palleon::CGraphicDevice::GetInstance().RemoveViewport(m_mainViewport.get());
	Palleon::CGraphicDevice::GetInstance().RemoveViewport(m_uiViewport.get());
}

void CApplication::CreateScene()
{
	auto screenSize = Palleon::CGraphicDevice::GetInstance().GetScreenSize();

	m_mainViewport = Palleon::CViewport::Create();

	{
		auto camera = CTouchFreeCamera::Create();
		camera->SetPerspectiveProjection(M_PI / 4, screenSize.x / screenSize.y, 1, 1000);
//		camera->SetOrthoProjection(100, 100, 1, 1000);
		camera->SetPosition(CVector3(0, 0, -45));
		m_mainViewport->SetCamera(camera);
		m_mainCamera = camera;
	}

	auto sceneRoot = m_mainViewport->GetSceneRoot();

	for(int i = 0; i < 5; i++)
	{
		auto cube = Palleon::CCubeMesh::Create();
		cube->SetPosition(CVector3(-80 + (i * 40), 0, 0));
		cube->SetScale(CVector3(15, 40, 10));
		cube->GetMaterial()->SetColor(CColor(1, 0, 0, 1));
		sceneRoot->AppendChild(cube);
		m_occluders.push_back(cube);
	}

	static int sphereCount = 40;
	static float ringRadius = 200.f;
	static float ringPosition = 300.f;
	float angleStep = (2 * M_PI) / static_cast<float>(sphereCount);
	for(int i = 0; i < sphereCount; i++)
	{
		float angle = angleStep * i;
		auto sphere = Palleon::CSphereMesh::Create();
		sphere->SetPosition(CVector3(ringRadius * sin(angle), 0, ringPosition + ringRadius * cos(angle)));
		sphere->SetScale(CVector3(3, 3, 3));
		sphere->GetMaterial()->SetColor(CColor(0, 1, 0, 1));
		sceneRoot->AppendChild(sphere);
		m_occludees.push_back(sphere);
	}
}

void CApplication::CreateUi()
{
	auto screenSize = Palleon::CGraphicDevice::GetInstance().GetScreenSize();

	m_uiViewport = Palleon::CViewport::Create();

	{
		auto camera = Palleon::CCamera::Create();
		camera->SetupOrthoCamera(screenSize.x, screenSize.y);
		m_uiViewport->SetCamera(camera);
	}

	{
		auto sceneRoot = m_uiViewport->GetSceneRoot();

		{
			auto scene = Palleon::CScene::Create(Palleon::CResourceManager::GetInstance().GetResource<Palleon::CSceneDescriptor>("main_scene.xml"));

			m_timeMetricLabel = scene->FindNode<Palleon::CLabel>("TimeMetricLabel");
			m_passCountMetricLabel = scene->FindNode<Palleon::CLabel>("PassCountMetricLabel");

			{
				auto sprite = scene->FindNode<Palleon::CSprite>("BackwardSprite");
				m_backwardButtonBoundingBox.position = sprite->GetPosition().xy();
				m_backwardButtonBoundingBox.size = sprite->GetSize();
			}

			{
				auto sprite = scene->FindNode<Palleon::CSprite>("ForwardSprite");
				m_forwardButtonBoundingBox.position = sprite->GetPosition().xy();
				m_forwardButtonBoundingBox.size = sprite->GetSize();
			}

			sceneRoot->AppendChild(scene);
		}
	}
}

void CApplication::UpdateMetrics()
{
	double totalTime = 0;
	for(const auto& time : m_renderTimes)
	{
		totalTime += time;
	}
	totalTime /= m_renderTimes.size();
	m_timeMetricLabel->SetText(string_format("Occlusion Test Render Time: %0.2fms", totalTime * 1000.f));
}

void CApplication::Update(float dt)
{
	m_mainCamera->Update(dt);
	m_mainViewport->GetSceneRoot()->Update(dt);
	m_mainViewport->GetSceneRoot()->UpdateTransformations();
	m_uiViewport->GetSceneRoot()->Update(dt);
	m_uiViewport->GetSceneRoot()->UpdateTransformations();

	{
		LARGE_INTEGER freq, begin, end;
		QueryPerformanceFrequency(&freq);
		QueryPerformanceCounter(&begin);
		m_occlusionBuffer.Clear(1.0f);
		m_occlusionBuffer.SetCamera(m_mainCamera);
		for(const auto& occluder : m_occluders)
		{
			m_occlusionBuffer.DrawMesh(occluder);
		}
		uint32 passCount = 0;
		for(const auto& occludee : m_occludees)
		{
			if(m_occlusionBuffer.TestMesh(occludee))
			{
				passCount++;
			}
		}
		QueryPerformanceCounter(&end);
		auto delta = end.QuadPart - begin.QuadPart;
		auto time = static_cast<double>(delta) / static_cast<double>(freq.QuadPart);
		m_renderTimes.pop_back();
		m_renderTimes.push_front(time);
		m_passCountMetricLabel->SetText(string_format("Occlusion Test Pass Count: %d out of %d", passCount, m_occludees.size()));
//		Framework::CBMP::WriteBitmap(m_occlusionBuffer.MakeBufferBitmap(), Framework::CreateOutputStdStream(std::string("occbuf.bmp")));
	}

	m_updateTimeMetricTimer += dt;
	if(m_updateTimeMetricTimer > 1)
	{
		UpdateMetrics();
		m_updateTimeMetricTimer -= 1;
	}
}

void CApplication::NotifyMouseMove(int x, int y)
{
	m_mousePosition = CVector2(x, y);
	m_mainCamera->NotifyMouseMove(x, y);
}

void CApplication::NotifyMouseDown()
{
	Palleon::CInputManager::SendInputEventToTree(m_uiViewport->GetSceneRoot(), m_mousePosition, Palleon::INPUT_EVENT_PRESSED);
	if(m_forwardButtonBoundingBox.Intersects(CBox2(m_mousePosition.x, m_mousePosition.y, 4, 4)))
	{
		m_mainCamera->NotifyMouseDown_MoveForward();
	}
	else if(m_backwardButtonBoundingBox.Intersects(CBox2(m_mousePosition.x, m_mousePosition.y, 4, 4)))
	{
		m_mainCamera->NotifyMouseDown_MoveBackward();
	}
	else
	{
		m_mainCamera->NotifyMouseDown_Center();
	}
}

void CApplication::NotifyMouseUp()
{
	Palleon::CInputManager::SendInputEventToTree(m_uiViewport->GetSceneRoot(), m_mousePosition, Palleon::INPUT_EVENT_RELEASED);
	m_mainCamera->NotifyMouseUp();
}

Palleon::CApplication* CreateApplication(bool)
{
	return new CApplication();
}
