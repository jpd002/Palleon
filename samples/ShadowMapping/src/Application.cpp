#include "Application.h"

using namespace ShadowMapping;

CApplication::CApplication()
: m_mousePosition(0, 0)
, m_elapsed(0)
{
	m_globalPackage = Athena::CPackage::Create("global");

	CreateScene();
	CreateUi();

	Athena::CGraphicDevice::GetInstance().AddViewport(m_mainViewport.get());
	Athena::CGraphicDevice::GetInstance().AddViewport(m_uiViewport.get());
}

CApplication::~CApplication()
{
	Athena::CGraphicDevice::GetInstance().RemoveViewport(m_mainViewport.get());
	Athena::CGraphicDevice::GetInstance().RemoveViewport(m_uiViewport.get());
}

void CApplication::CreateScene()
{
	auto screenSize = Athena::CGraphicDevice::GetInstance().GetScreenSize();

	m_mainViewport = Athena::CViewport::Create();

	{
		auto camera = CTouchFreeCamera::Create();
		camera->SetPerspectiveProjection(M_PI / 4, screenSize.x / screenSize.y, 1, 10000);
		camera->SetPosition(CVector3(350, 700, -350));
		camera->SetHorizontalAngle(-M_PI / 4.f);
		camera->SetVerticalAngle(5.f * M_PI / 16.f);
		m_mainViewport->SetCamera(camera);
		m_mainCamera = camera;
	}

	{
		auto camera = Athena::CCamera::Create();
//		camera->SetOrthoProjection(512, 512, 1, 1000);
		camera->SetPerspectiveProjection(M_PI / 2, 1, 100, 1000);
		m_mainViewport->SetShadowCamera(camera);
		m_shadowCamera = camera;
	}

//	m_mainViewport->SetCamera(m_shadowCamera);

	auto sceneRoot = m_mainViewport->GetSceneRoot();

	static const float areaSize = 256;
	static const int sphereCount = 50;

	{
		auto cube = Athena::CCubeMesh::Create();
		cube->SetScale(CVector3(areaSize, 50, areaSize));
		cube->SetPosition(CVector3(0, -200, 0));
		cube->GetMaterial()->SetColor(CColor(0, 0, 1, 1));
		cube->GetMaterial()->SetShadowReceiving(true);
		sceneRoot->AppendChild(cube);
	}
	
#ifdef BOUNDING_BOX
	{
		CQuaternion rot(CVector3(0, 0, 1), -M_PI / 2);
		auto mat = rot.ToMatrix();
//		auto rot2 = CQuaternion(mat);

		auto view = m_shadowCamera->GetViewMatrix();
		view(3, 0) = 0;
		view(3, 1) = 0;
		view(3, 2) = 0;
		auto invView = view.Inverse();
		auto quat = CQuaternion(invView);
		CVector3 axisX(1, 0, 0);
		CVector3 axisY(0, 1, 0);
		CVector3 axisZ(0, 0, 1);
		auto resX = axisX * invView;
		auto resY = axisY * invView;
		auto resZ = axisZ * invView;
		quat.Normalize();

		auto shadowProjVolume = Athena::CCubeMesh::Create();
//		shadowProjVolume->SetPosition(CVector3(0, 200, 0));
		shadowProjVolume->SetRotation(quat);
		shadowProjVolume->SetScale(CVector3(512 / 2, 512 / 2, 999));
		shadowProjVolume->GetMaterial()->SetColor(CColor(0.5f, 0.5f, 0.5f, 0.5f));
		shadowProjVolume->GetMaterial()->SetAlphaBlendingMode(Athena::ALPHA_BLENDING_LERP);
		sceneRoot->AppendChild(shadowProjVolume);
	}
#endif

	//Show camera position
	{
		auto sphere = Athena::CSphereMesh::Create();
		sphere->SetScale(CVector3(10, 10, 10));
		sphere->GetMaterial()->SetColor(CColor(0, 1, 0, 1));
		sceneRoot->AppendChild(sphere);
		m_shadowCameraSphere = sphere;
	}

	srand(10);

	for(unsigned int i = 0; i < sphereCount; i++)
	{
		float xPos = ((static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) - 0.5f) * 2.f * areaSize;
		float zPos = ((static_cast<float>(rand()) / static_cast<float>(RAND_MAX)) - 0.5f) * 2.f * areaSize;

		auto sphere = Athena::CSphereMesh::Create();
		sphere->SetScale(CVector3(20, 20, 20));
		sphere->SetPosition(CVector3(xPos, 0, zPos));
		sphere->GetMaterial()->SetColor(CColor(1, 0, 0, 1));
		sphere->GetMaterial()->SetShadowCasting(true);
		sceneRoot->AppendChild(sphere);
	}
}

void CApplication::CreateUi()
{
	auto screenSize = Athena::CGraphicDevice::GetInstance().GetScreenSize();

	m_uiViewport = Athena::CViewport::Create();

	{
		auto camera = Athena::CCamera::Create();
		camera->SetupOrthoCamera(screenSize.x, screenSize.y);
		m_uiViewport->SetCamera(camera);
	}

	{
		auto sceneRoot = m_uiViewport->GetSceneRoot();

		{
			auto scene = Athena::CScene::Create(Athena::CResourceManager::GetInstance().GetResource<Athena::CSceneDescriptor>("main_scene.xml"));

			{
				auto sprite = scene->FindNode<Athena::CSprite>("BackwardSprite");
				m_backwardButtonBoundingBox.position = sprite->GetPosition().xy();
				m_backwardButtonBoundingBox.size = sprite->GetSize();
			}

			{
				auto sprite = scene->FindNode<Athena::CSprite>("ForwardSprite");
				m_forwardButtonBoundingBox.position = sprite->GetPosition().xy();
				m_forwardButtonBoundingBox.size = sprite->GetSize();
			}

			sceneRoot->AppendChild(scene);
		}
	}
}

void CApplication::Update(float dt)
{
	UpdateShadowCamera();
	m_mainCamera->Update(dt);
	m_mainViewport->GetSceneRoot()->Update(dt);
	m_mainViewport->GetSceneRoot()->UpdateTransformations();
	m_uiViewport->GetSceneRoot()->Update(dt);
	m_uiViewport->GetSceneRoot()->UpdateTransformations();
	m_elapsed += dt;
}

void CApplication::UpdateShadowCamera()
{
	CVector3 shadowCameraPosition(0, 384.f + 128.f * sin(m_elapsed), 0);
	m_shadowCamera->LookAt(shadowCameraPosition, CVector3(0, 0, 0), CVector3(0, 0, -1));
	m_shadowCameraSphere->SetPosition(shadowCameraPosition);
}

void CApplication::NotifyMouseMove(int x, int y)
{
	m_mousePosition = CVector2(x, y);
	m_mainCamera->NotifyMouseMove(x, y);
}

void CApplication::NotifyMouseDown()
{
	Athena::CInputManager::SendInputEventToTree(m_uiViewport->GetSceneRoot(), m_mousePosition, Athena::INPUT_EVENT_PRESSED);
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
	Athena::CInputManager::SendInputEventToTree(m_uiViewport->GetSceneRoot(), m_mousePosition, Athena::INPUT_EVENT_RELEASED);
	m_mainCamera->NotifyMouseUp();
}

Athena::CApplication* CreateApplication()
{
	return new CApplication();
}
