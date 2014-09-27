#include "Application.h"
#include "Dx11WaterEffectProvider.h"
#include "palleon/win32/Dx11GraphicDevice.h"

using namespace Water;

#define MAIN_CAMERA_FOV		(M_PI / 3.f)
#define MAIN_CAMERA_NEAR_Z	(12.f)
#define MAIN_CAMERA_FAR_Z	(4000.f)

CApplication::CApplication()
: m_mousePosition(0, 0)
, m_elapsed(0)
{
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
		camera->SetPerspectiveProjection(MAIN_CAMERA_FOV, screenSize.x / screenSize.y, MAIN_CAMERA_NEAR_Z, MAIN_CAMERA_FAR_Z, Palleon::HANDEDNESS_RIGHTHANDED);
		camera->SetPosition(CVector3(350, 700, -350));
		camera->SetHorizontalAngle(3.f * M_PI / 4.f);
		camera->SetVerticalAngle(-5.f * M_PI / 16.f);
		m_mainViewport->SetCamera(camera);
		m_mainCamera = camera;
	}

	{
		auto camera = Palleon::CCamera::Create();
		m_reflectCamera = camera;
	}

	auto sceneRoot = m_mainViewport->GetSceneRoot();

	static const float areaSize = 1024;

	{
		auto& graphicDevice = static_cast<Palleon::CDx11GraphicDevice&>(Palleon::CGraphicDevice::GetInstance());
		m_waterEffectProvider = std::make_shared<CDx11WaterEffectProvider>(graphicDevice.GetDevice(), graphicDevice.GetDeviceContext());
	}

	const unsigned int targetSize = 512;
	m_reflectRenderTarget = Palleon::CGraphicDevice::GetInstance().CreateRenderTarget(Palleon::TEXTURE_FORMAT_RGBA8888, targetSize, targetSize);
	m_refractRenderTarget = Palleon::CGraphicDevice::GetInstance().CreateRenderTarget(Palleon::TEXTURE_FORMAT_RGBA8888, targetSize, targetSize);
	auto waterBumpTexture = Palleon::CResourceManager::GetInstance().GetTexture("waterbump.png");

	//Depths
	{
		auto cube = Palleon::CCubeMesh::Create();
		cube->SetScale(CVector3(areaSize, 50, areaSize));
		cube->SetPosition(CVector3(0, -200, 0));
		cube->GetMaterial()->SetColor(CColor(0, 0, 1, 1));
		sceneRoot->AppendChild(cube);
	}

	//Pillar
	{
		auto cube = Palleon::CCubeMesh::Create();
		cube->SetScale(CVector3(areaSize * 0.10f, 300, areaSize * 0.10f));
		cube->SetPosition(CVector3(0, -100, 0));
		cube->GetMaterial()->SetColor(CColor(0, 1, 0, 1));
		cube->GetMaterial()->SetCullingMode(Palleon::CULLING_CW);
		sceneRoot->AppendChild(cube);
	}

	//Water
	{
		auto plane = Palleon::CCubeMesh::Create();
		plane->SetScale(CVector3(areaSize * 2, 0, areaSize * 2));
		plane->SetPosition(CVector3(0, 0, 0));
		plane->GetMaterial()->SetCullingMode(Palleon::CULLING_CW);
		plane->GetMaterial()->SetTexture(0, m_refractRenderTarget);
		plane->GetMaterial()->SetTexture(1, waterBumpTexture);
		plane->GetMaterial()->SetTextureAddressModeU(1, Palleon::TEXTURE_ADDRESS_REPEAT);
		plane->GetMaterial()->SetTextureAddressModeV(1, Palleon::TEXTURE_ADDRESS_REPEAT);
		plane->SetEffectProvider(m_waterEffectProvider);
		sceneRoot->AppendChild(plane);
		m_waterPlane = plane;
	}
}

void CApplication::CreateUi()
{
	m_uiViewport = Palleon::CViewport::Create();

	{
		auto camera = Palleon::CCamera::Create();
		m_uiViewport->SetCamera(camera);
	}

	auto sceneRoot = m_uiViewport->GetSceneRoot();

	{
		auto scene = Palleon::CScene::Create(Palleon::CResourceManager::GetInstance().GetResource<Palleon::CSceneDescriptor>("main_scene.xml"));
		sceneRoot->AppendChild(scene);
		m_uiScene = scene;
	}

	RefreshUiLayout();
}

float sgn(float a)
{
	if(a > 0.f) return 1.f;
	if(a < 0.f) return -1.f;
	return 0.f;
}

void CApplication::Update(float dt)
{
	m_mainCamera->Update(dt);
	m_mainViewport->GetSceneRoot()->Update(dt);
	m_mainViewport->GetSceneRoot()->UpdateTransformations();
	m_uiViewport->GetSceneRoot()->Update(dt);
	m_uiViewport->GetSceneRoot()->UpdateTransformations();

	{
		float waterScale = 10;
		auto waterBumpTextureMatrix = CMatrix4::MakeScale(waterScale, waterScale, 1) * CMatrix4::MakeTranslation(m_elapsed / 25.f, m_elapsed / 50.f, 0);
		m_waterPlane->GetMaterial()->SetTextureMatrix(1, waterBumpTextureMatrix);
	}

	{
		{
			auto invViewMatrix = m_mainCamera->GetViewMatrix().Inverse();
			auto invProjMatrix = m_mainCamera->GetProjectionMatrix().Inverse();

			auto clipPlane = CVector4(0, -1, 0, 0);
			auto clipPlaneView = clipPlane * invViewMatrix;
		
			assert(clipPlaneView.w < 0);

			auto clipSpaceCorner = CVector4(sgn(clipPlaneView.x), sgn(clipPlaneView.y), 1, 1);
			clipSpaceCorner = clipSpaceCorner * invProjMatrix;

			//This maybe depends on the graphics API because of the depth range
			auto c = clipPlaneView * (1.f / clipPlaneView.Dot(clipSpaceCorner));

			auto projMatrix = m_mainCamera->GetProjectionMatrix();
			projMatrix(0, 2) = c.x;
			projMatrix(1, 2) = c.y;
			projMatrix(2, 2) = c.z;
			projMatrix(3, 2) = c.w;

			m_reflectCamera->SetViewMatrix(m_mainCamera->GetViewMatrix());
			m_reflectCamera->SetProjectionMatrix(projMatrix);
		}

		m_mainViewport->SetCamera(m_reflectCamera);
		m_waterPlane->SetVisible(false);
		m_refractRenderTarget->Draw(m_mainViewport);
		m_waterPlane->SetVisible(true);
		m_mainViewport->SetCamera(m_mainCamera);
	}

	m_elapsed += dt;
}

void CApplication::RefreshUiLayout()
{
	auto screenSize = Palleon::CGraphicDevice::GetInstance().GetScreenSize();

	{
		auto camera = m_uiViewport->GetCamera();
		camera->SetupOrthoCamera(screenSize.x, screenSize.y);
	}

	{
		auto sceneLayout = m_uiScene->GetLayout();
		sceneLayout->SetRect(10, 10, screenSize.x - 10, screenSize.y - 10);
		sceneLayout->RefreshGeometry();
	}

	{
		auto sprite = m_uiScene->FindNode<Palleon::CSprite>("BackwardSprite");
		m_backwardButtonBoundingBox.position = sprite->GetPosition().xy();
		m_backwardButtonBoundingBox.size = sprite->GetSize();
	}

	{
		auto sprite = m_uiScene->FindNode<Palleon::CSprite>("ForwardSprite");
		m_forwardButtonBoundingBox.position = sprite->GetPosition().xy();
		m_forwardButtonBoundingBox.size = sprite->GetSize();
	}
}

void CApplication::NotifySizeChanged()
{
	auto screenSize = Palleon::CGraphicDevice::GetInstance().GetScreenSize();
	m_mainCamera->SetPerspectiveProjection(MAIN_CAMERA_FOV, screenSize.x / screenSize.y, MAIN_CAMERA_NEAR_Z, MAIN_CAMERA_FAR_Z, Palleon::HANDEDNESS_RIGHTHANDED);
	RefreshUiLayout();
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
	palleon_library_link();		//Needed for Android to work properly
	return new CApplication();
}
