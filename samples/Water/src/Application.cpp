#include "Application.h"
#include "WaterEffectProvider.h"

using namespace Water;

#define MAIN_CAMERA_FOV		(M_PI / 3.f)
#define MAIN_CAMERA_NEAR_Z	(12.f)
#define MAIN_CAMERA_FAR_Z	(4000.f)

CApplication::CApplication()
{
	m_globalPackage = Palleon::CPackage::Create("global");

	CreateScene();
	CreateSky();
	CreateUi();

	Palleon::CGraphicDevice::GetInstance().AddViewport(m_skyViewport.get());
	Palleon::CGraphicDevice::GetInstance().AddViewport(m_mainViewport.get());
	Palleon::CGraphicDevice::GetInstance().AddViewport(m_uiViewport.get());
}

CApplication::~CApplication()
{
	Palleon::CGraphicDevice::GetInstance().RemoveViewport(m_skyViewport.get());
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

	static const float areaSize = 2048;

	{
		m_waterEffectProvider = std::make_shared<CWaterEffectProvider>();
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
		cube->GetMaterial()->SetColor(CColor(0, 32.f / 255.f, 0, 1));
		cube->GetMaterial()->SetCullingMode(Palleon::CULLING_CW);
		sceneRoot->AppendChild(cube);
	}

	//Pillar
	{
		auto cube = Palleon::CCubeMesh::Create();
		cube->SetScale(CVector3(areaSize * 0.05f, 300, areaSize * 0.05f));
		cube->SetPosition(CVector3(0, -100, 0));
		cube->GetMaterial()->SetColor(CColor(0, 1, 0, 1));
		cube->GetMaterial()->SetCullingMode(Palleon::CULLING_CW);
		sceneRoot->AppendChild(cube);
	}

	//Ball
	{
		auto sphere = Palleon::CSphereMesh::Create();
		sphere->SetScale(CVector3(50, 50, 50));
		sphere->SetPosition(CVector3(0, 200, 0));
		sphere->GetMaterial()->SetCullingMode(Palleon::CULLING_CW);
		sceneRoot->AppendChild(sphere);
		m_overBall = sphere;
	}

	//Ball
	{
		auto sphere = Palleon::CSphereMesh::Create();
		sphere->SetScale(CVector3(50, 50, 50));
		sphere->SetPosition(CVector3(0, -100, 0));
		sphere->GetMaterial()->SetColor(CColor(1, 0, 0, 1));
		sphere->GetMaterial()->SetCullingMode(Palleon::CULLING_CW);
		sceneRoot->AppendChild(sphere);
		m_underBall = sphere;
	}

	//Water
	{
		auto plane = Palleon::CCubeMesh::Create();
		plane->SetScale(CVector3(areaSize, 1, areaSize));
		plane->SetPosition(CVector3(0, 0, 0));
		plane->GetMaterial()->SetCullingMode(Palleon::CULLING_CW);
		plane->GetMaterial()->SetTexture(0, m_reflectRenderTarget);
		plane->GetMaterial()->SetTexture(1, m_refractRenderTarget);
		plane->GetMaterial()->SetTexture(2, waterBumpTexture);
		plane->GetMaterial()->SetTexture(3, waterBumpTexture);
		plane->GetMaterial()->SetTextureAddressModeU(2, Palleon::TEXTURE_ADDRESS_REPEAT);
		plane->GetMaterial()->SetTextureAddressModeV(2, Palleon::TEXTURE_ADDRESS_REPEAT);
		plane->GetMaterial()->SetTextureAddressModeU(3, Palleon::TEXTURE_ADDRESS_REPEAT);
		plane->GetMaterial()->SetTextureAddressModeV(3, Palleon::TEXTURE_ADDRESS_REPEAT);
		plane->SetEffectProvider(m_waterEffectProvider);
		sceneRoot->AppendChild(plane);
		m_waterPlane = plane;
	}
}

void CApplication::CreateSky()
{
	{
		auto skyTextureStream = Palleon::CResourceManager::GetInstance().MakeResourceStream("global/skybox.dds");
		m_skyTexture = Palleon::CTextureLoader::CreateCubeTextureFromStream(*skyTextureStream.get());
	}

	{
		auto viewport = Palleon::CViewport::Create();
		m_skyViewport = viewport;
	}

	{
		auto camera = Palleon::CCamera::Create();
		m_skyViewport->SetCamera(camera);
		m_skyCamera = camera;
	}

	auto sceneRoot = m_skyViewport->GetSceneRoot();

	//Create skybox
	{
		auto skyBox = Palleon::CCubeMesh::Create();
		skyBox->SetScale(CVector3(100, 100, 100));
		skyBox->GetMaterial()->SetCullingMode(Palleon::CULLING_NONE);
		skyBox->GetMaterial()->SetTexture(0, m_skyTexture);
		skyBox->GetMaterial()->SetTextureCoordSource(0, Palleon::TEXTURE_COORD_CUBE_POS);
		sceneRoot->AppendChild(skyBox);
		m_skyBox = skyBox;
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

CMatrix4 MakeObliqueClippedFrustum(const CMatrix4& viewMatrix, const CMatrix4& projMatrix, const CVector4& clipPlane)
{
	auto invViewMatrix = viewMatrix.Inverse();
	auto invProjMatrix = projMatrix.Inverse();

	auto clipPlaneView = clipPlane * invViewMatrix;
		
	//assert(clipPlaneView.w < 0);

	auto clipSpaceCorner = CVector4(sgn(clipPlaneView.x), sgn(clipPlaneView.y), 1, 1);
	clipSpaceCorner = clipSpaceCorner * invProjMatrix;

	//This depends on the graphics API because of the depth range
#if defined(PALLEON_WIN32)
	auto c = clipPlaneView * (1.f / clipPlaneView.Dot(clipSpaceCorner));
#elif defined(PALLEON_IOS) || defined(PALLEON_ANDROID)
	auto c = clipPlaneView * (2.f / clipPlaneView.Dot(clipSpaceCorner));
#endif

	auto clippedProjMatrix = projMatrix;
	clippedProjMatrix(0, 2) = c.x;
	clippedProjMatrix(1, 2) = c.y;
#if defined(PALLEON_WIN32)
	clippedProjMatrix(2, 2) = c.z;
#elif defined(PALLEON_IOS) || defined(PALLEON_ANDROID)
	clippedProjMatrix(2, 2) = c.z + 1.f;
#endif
	clippedProjMatrix(3, 2) = c.w;
	return clippedProjMatrix;
}

void CApplication::Update(float dt)
{
	m_overBall->SetPosition(CVector3(300.f * cos(m_elapsed / 2.5f), 200.f, 300.f * sin(m_elapsed / 2.5f)));
	m_underBall->SetPosition(CVector3(200.f * cos(m_elapsed / 1.5f), -100.f, 400.f * sin(m_elapsed / 1.5f)));

	m_mainCamera->Update(dt);
	m_mainViewport->GetSceneRoot()->Update(dt);
	m_mainViewport->GetSceneRoot()->UpdateTransformations();
	m_skyViewport->GetSceneRoot()->Update(dt);
	m_skyViewport->GetSceneRoot()->UpdateTransformations();
	m_uiViewport->GetSceneRoot()->Update(dt);
	m_uiViewport->GetSceneRoot()->UpdateTransformations();

	{
		float waterScale = 6;
		float transX = static_cast<float>(m_elapsedInt % 10000) / 10000.f;
		float transY = static_cast<float>(m_elapsedInt % 12000) / 12000.f;
		auto waterBumpTextureMatrix = CMatrix4::MakeScale(waterScale, waterScale, 1) * CMatrix4::MakeTranslation(transX, transY, 0);
		m_waterPlane->GetMaterial()->SetTextureMatrix(2, waterBumpTextureMatrix);
	}

	{
		float waterScale = 10;
		float transX = static_cast<float>(m_elapsedInt % 10000) / 10000.f;
		float transY = static_cast<float>(m_elapsedInt % 14000) / 14000.f;
		auto waterBumpTextureMatrix = CMatrix4::MakeScale(waterScale, waterScale, 1) * CMatrix4::MakeTranslation(-transX, transY, 0);
		m_waterPlane->GetMaterial()->SetTextureMatrix(3, waterBumpTextureMatrix);
	}

	m_waterPlane->SetVisible(false);

	RenderReflection();
	RenderRefraction();

	//Reset the state
	m_waterPlane->SetVisible(true);
	m_mainViewport->SetCamera(m_mainCamera);

	SetupSkyCamera(m_mainCamera);
	m_skyViewport->SetCamera(m_skyCamera);

	m_elapsed += dt;
	m_elapsedInt += (dt * 1000.f);
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

void CApplication::RenderReflection()
{
	auto clipPlane = CVector4(0, 1, 0, 0);
	auto reflectMatrix = CMatrix4::MakeReflect(clipPlane.x, clipPlane.y, clipPlane.z, clipPlane.w);

	m_reflectRenderTarget->Clear();

	SetupSkyCamera(m_mainCamera);

	{
		auto viewMatrix = reflectMatrix * m_skyCamera->GetViewMatrix();
		auto projMatrix = MakeObliqueClippedFrustum(viewMatrix, m_skyCamera->GetProjectionMatrix(), clipPlane);

		m_reflectCamera->SetViewMatrix(viewMatrix);
		m_reflectCamera->SetProjectionMatrix(projMatrix);
	}
	m_skyViewport->SetCamera(m_reflectCamera);
	m_reflectRenderTarget->Draw(m_skyViewport);

	{
		auto viewMatrix = reflectMatrix * m_mainCamera->GetViewMatrix();
		auto projMatrix = MakeObliqueClippedFrustum(viewMatrix, m_mainCamera->GetProjectionMatrix(), clipPlane);

		m_reflectCamera->SetViewMatrix(viewMatrix);
		m_reflectCamera->SetProjectionMatrix(projMatrix);
	}
	m_mainViewport->SetCamera(m_reflectCamera);
	m_reflectRenderTarget->Draw(m_mainViewport);
}

void CApplication::RenderRefraction()
{
	{
		auto clipPlane = CVector4(0, -1, 0, 0);

		auto viewMatrix = m_mainCamera->GetViewMatrix();
		auto projMatrix = MakeObliqueClippedFrustum(viewMatrix, m_mainCamera->GetProjectionMatrix(), clipPlane);

		m_reflectCamera->SetViewMatrix(viewMatrix);
		m_reflectCamera->SetProjectionMatrix(projMatrix);
	}

	m_mainViewport->SetCamera(m_reflectCamera);
	m_refractRenderTarget->Clear();
	m_refractRenderTarget->Draw(m_mainViewport);
}

void CApplication::SetupSkyCamera(const Palleon::CameraPtr& camera)
{
	m_skyCamera->SetProjectionMatrix(camera->GetProjectionMatrix());
	auto viewMatrix = camera->GetViewMatrix();
	viewMatrix(3, 0) = 0;
	viewMatrix(3, 1) = -10;
	viewMatrix(3, 2) = 0;
	m_skyCamera->SetViewMatrix(viewMatrix);
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
