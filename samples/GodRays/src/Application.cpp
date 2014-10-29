#include "Application.h"
#include "FullscreenQuad.h"
#include "OcclusionMapEffect.h"
#include "GodRayBlurEffect.h"
#include "GodRayCompositeEffect.h"

using namespace GodRays;

#define MAIN_CAMERA_FOV		(M_PI / 3.f)
#define MAIN_CAMERA_NEAR_Z	(10.f)
#define MAIN_CAMERA_FAR_Z	(10000.f)

CApplication::CApplication()
{
	m_globalPackage = Palleon::CPackage::Create("global");

	m_occlusionMapEffectProvider = std::make_shared<Palleon::CGenericEffectProvider<COcclusionMapEffect>>();
	m_godRayBlurEffectProvider = std::make_shared<Palleon::CGenericEffectProvider<CGodRayBlurEffect>>();
	m_godRayCompositeEffectProvider = std::make_shared<Palleon::CGenericEffectProvider<CGodRayCompositeEffect>>();

	auto screenSize = Palleon::CGraphicDevice::GetInstance().GetScaledScreenSize();
	auto postScreenSize = screenSize / 4.f;

	m_sceneRenderTarget = Palleon::CGraphicDevice::GetInstance().CreateRenderTarget(Palleon::TEXTURE_FORMAT_BGRA8888, screenSize.x, screenSize.y);
	m_unblurredRenderTarget = Palleon::CGraphicDevice::GetInstance().CreateRenderTarget(Palleon::TEXTURE_FORMAT_BGRA8888, postScreenSize.x, postScreenSize.y);
	m_blurredRenderTarget = Palleon::CGraphicDevice::GetInstance().CreateRenderTarget(Palleon::TEXTURE_FORMAT_BGRA8888, postScreenSize.x, postScreenSize.y);
	m_finalRenderTarget = Palleon::CGraphicDevice::GetInstance().CreateRenderTarget(Palleon::TEXTURE_FORMAT_BGRA8888, postScreenSize.x, postScreenSize.y);

	CreateScene();
	CreatePostProcess();
	CreateUi();

	Palleon::CGraphicDevice::GetInstance().AddViewport(m_postProcessViewport.get());
	Palleon::CGraphicDevice::GetInstance().AddViewport(m_uiViewport.get());
}

CApplication::~CApplication()
{
	Palleon::CGraphicDevice::GetInstance().RemoveViewport(m_postProcessViewport.get());
	Palleon::CGraphicDevice::GetInstance().RemoveViewport(m_uiViewport.get());
}

void CApplication::CreateScene()
{
	auto screenSize = Palleon::CGraphicDevice::GetInstance().GetScreenSize();

	m_sceneViewport = Palleon::CViewport::Create();

	{
		auto camera = CTouchFreeCamera::Create();
		camera->SetPerspectiveProjection(MAIN_CAMERA_FOV, screenSize.x / screenSize.y, MAIN_CAMERA_NEAR_Z, MAIN_CAMERA_FAR_Z);
		camera->SetPosition(CVector3(2000.f, 100.f, 0.f));
		camera->SetHorizontalAngle(-M_PI / 2.f);
		camera->SetVerticalAngle(0);
		m_sceneViewport->SetCamera(camera);
		m_sceneCamera = camera;
	}

	auto sceneRoot = m_sceneViewport->GetSceneRoot();

	static const float areaSize = 2048;

	//Depths
	{
		auto cube = Palleon::CCubeMesh::Create();
		cube->SetScale(CVector3(areaSize, 50, areaSize));
		cube->SetPosition(CVector3(0, -200, 0));
		cube->GetMaterial()->SetColor(CColor(0, 32.f / 255.f, 0, 1));
		sceneRoot->AppendChild(cube);
	}

	//Pillar
	for(int i = -3; i < 4; i++)
	{
		auto cube = Palleon::CCubeMesh::Create();
		cube->SetScale(CVector3(areaSize * 0.05f, 1000, areaSize * 0.05f));
		cube->SetPosition(CVector3(0, -100, (i * areaSize * 0.25f)));
		cube->GetMaterial()->SetColor(CColor(0, 1, 0, 1));
		sceneRoot->AppendChild(cube);
	}

	//Ball
	{
		auto sphere = Palleon::CSphereMesh::Create();
		sphere->SetScale(CVector3(600, 600, 600));
		sceneRoot->AppendChild(sphere);
		m_lightNode = sphere;
	}
}

void CApplication::CreatePostProcess()
{
	m_postProcessViewport = Palleon::CViewport::Create();
	m_postProcessViewport->SetCamera(Palleon::CCamera::Create());

	auto sceneRoot = m_postProcessViewport->GetSceneRoot();

	{
		auto fsQuad = Palleon::CFullscreenQuad::Create();
		sceneRoot->AppendChild(fsQuad);
		m_postProcessMesh = fsQuad;
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

void CApplication::Update(float dt)
{
	m_lightNode->SetPosition(CVector3(-2000, 1500, 2500 * sin(m_elapsed / 5.f)));

	m_sceneCamera->Update(dt);
	m_sceneViewport->GetSceneRoot()->Update(dt);
	m_sceneViewport->GetSceneRoot()->UpdateTransformations();

	//Draw scene in RT
	{
		m_sceneViewport->GetSceneRoot()->TraverseNodes(
			[&](const Palleon::SceneNodePtr& sceneNode)
			{
				if(sceneNode == m_lightNode)
				{
					sceneNode->SetVisible(false);
				}
				if(auto mesh = std::dynamic_pointer_cast<Palleon::CMesh>(sceneNode))
				{
					mesh->SetEffectProvider(Palleon::CGraphicDevice::GetInstance().GetDefaultEffectProvider());
				}
				return true;
			}
		);
		m_sceneRenderTarget->Clear();
		m_sceneRenderTarget->Draw(m_sceneViewport);
	}

	//Draw occlusion map + down sampled scene
	{
		m_sceneViewport->GetSceneRoot()->TraverseNodes(
			[&](const Palleon::SceneNodePtr& sceneNode)
			{
				if(sceneNode == m_lightNode)
				{
					sceneNode->SetVisible(true);
					return true;
				}
				if(auto mesh = std::dynamic_pointer_cast<Palleon::CMesh>(sceneNode))
				{
					mesh->SetEffectProvider(m_occlusionMapEffectProvider);
				}
				return true;
			}
		);
		m_unblurredRenderTarget->Clear();
		m_unblurredRenderTarget->Draw(m_sceneViewport);
	}

	auto worldViewProjMatrix = m_lightNode->GetWorldTransformation() * m_sceneCamera->GetViewMatrix() * m_sceneCamera->GetProjectionMatrix();
	auto screenLightPos = worldViewProjMatrix * CVector4(0, 0, 0, 1);
	auto normalizedLightPos = screenLightPos.xyz() / screenLightPos.w;
#if defined(PALLEON_WIN32)
	auto texLightPos = (CVector3(normalizedLightPos.x, -normalizedLightPos.y, normalizedLightPos.z) / 2.f) + CVector3(0.5f, 0.5f, 0.5f);
#elif defined(PALLEON_IOS) || defined(PALLEON_ANDROID)
	auto texLightPos = (CVector3(normalizedLightPos.x, normalizedLightPos.y, normalizedLightPos.z) / 2.f) + CVector3(0.5f, 0.5f, 0.5f);
#endif
	{
		m_blurredRenderTarget->Clear();

		m_postProcessMesh->SetEffectProvider(m_godRayBlurEffectProvider);
		m_postProcessMesh->GetMaterial()->SetEffectParameter(CGodRayBlurEffect::g_screenLightPosUniformName, Palleon::CEffectParameter(texLightPos));
		m_postProcessMesh->GetMaterial()->SetEffectParameter(CGodRayBlurEffect::g_densityUniformName, Palleon::CEffectParameter(0.25f));
		m_postProcessMesh->GetMaterial()->SetTexture(0, m_unblurredRenderTarget);
		m_postProcessMesh->GetMaterial()->SetTexture(1, Palleon::TexturePtr());
		m_postProcessMesh->GetMaterial()->SetAlphaBlendingMode(Palleon::ALPHA_BLENDING_NONE);

		m_blurredRenderTarget->Draw(m_postProcessViewport);

		m_postProcessMesh->SetEffectProvider(m_godRayBlurEffectProvider);
		m_postProcessMesh->GetMaterial()->SetEffectParameter(CGodRayBlurEffect::g_screenLightPosUniformName, Palleon::CEffectParameter(texLightPos));
		m_postProcessMesh->GetMaterial()->SetEffectParameter(CGodRayBlurEffect::g_densityUniformName, Palleon::CEffectParameter(0.50f));
		m_postProcessMesh->GetMaterial()->SetTexture(0, m_unblurredRenderTarget);
		m_postProcessMesh->GetMaterial()->SetTexture(1, Palleon::TexturePtr());
		m_postProcessMesh->GetMaterial()->SetAlphaBlendingMode(Palleon::ALPHA_BLENDING_ADD);

		m_blurredRenderTarget->Draw(m_postProcessViewport);
	}

	{
		m_finalRenderTarget->Clear();

		m_postProcessMesh->SetEffectProvider(m_godRayBlurEffectProvider);
		m_postProcessMesh->GetMaterial()->SetEffectParameter(CGodRayBlurEffect::g_screenLightPosUniformName, Palleon::CEffectParameter(texLightPos));
		m_postProcessMesh->GetMaterial()->SetEffectParameter(CGodRayBlurEffect::g_densityUniformName, Palleon::CEffectParameter(0.75f));
		m_postProcessMesh->GetMaterial()->SetTexture(0, m_blurredRenderTarget);
		m_postProcessMesh->GetMaterial()->SetTexture(1, Palleon::TexturePtr());
		m_postProcessMesh->GetMaterial()->SetAlphaBlendingMode(Palleon::ALPHA_BLENDING_NONE);

		m_finalRenderTarget->Draw(m_postProcessViewport);

		m_postProcessMesh->SetEffectProvider(m_godRayBlurEffectProvider);
		m_postProcessMesh->GetMaterial()->SetEffectParameter(CGodRayBlurEffect::g_screenLightPosUniformName, Palleon::CEffectParameter(texLightPos));
		m_postProcessMesh->GetMaterial()->SetEffectParameter(CGodRayBlurEffect::g_densityUniformName, Palleon::CEffectParameter(1.00f));
		m_postProcessMesh->GetMaterial()->SetTexture(0, m_blurredRenderTarget);
		m_postProcessMesh->GetMaterial()->SetTexture(1, Palleon::TexturePtr());
		m_postProcessMesh->GetMaterial()->SetAlphaBlendingMode(Palleon::ALPHA_BLENDING_ADD);

		m_finalRenderTarget->Draw(m_postProcessViewport);
	}

	//Present pass
	{
		m_postProcessMesh->SetEffectProvider(m_godRayCompositeEffectProvider);
		m_postProcessMesh->GetMaterial()->SetTexture(0, m_sceneRenderTarget);
		m_postProcessMesh->GetMaterial()->SetTexture(1, m_finalRenderTarget);
		m_postProcessMesh->GetMaterial()->SetAlphaBlendingMode(Palleon::ALPHA_BLENDING_NONE);
	}

	m_postProcessViewport->GetSceneRoot()->Update(dt);
	m_postProcessViewport->GetSceneRoot()->UpdateTransformations();

	m_uiViewport->GetSceneRoot()->Update(dt);
	m_uiViewport->GetSceneRoot()->UpdateTransformations();

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

void CApplication::NotifySizeChanged()
{
	auto screenSize = Palleon::CGraphicDevice::GetInstance().GetScreenSize();
	m_sceneCamera->SetPerspectiveProjection(MAIN_CAMERA_FOV, screenSize.x / screenSize.y, MAIN_CAMERA_NEAR_Z, MAIN_CAMERA_FAR_Z);
	RefreshUiLayout();
}

void CApplication::NotifyMouseMove(int x, int y)
{
	m_mousePosition = CVector2(x, y);
	m_sceneCamera->NotifyMouseMove(x, y);
}

void CApplication::NotifyMouseDown()
{
	Palleon::CInputManager::SendInputEventToTree(m_uiViewport->GetSceneRoot(), m_mousePosition, Palleon::INPUT_EVENT_PRESSED);
	if(m_forwardButtonBoundingBox.Intersects(CBox2(m_mousePosition.x, m_mousePosition.y, 4, 4)))
	{
		m_sceneCamera->NotifyMouseDown_MoveForward();
	}
	else if(m_backwardButtonBoundingBox.Intersects(CBox2(m_mousePosition.x, m_mousePosition.y, 4, 4)))
	{
		m_sceneCamera->NotifyMouseDown_MoveBackward();
	}
	else
	{
		m_sceneCamera->NotifyMouseDown_Center();
	}
}

void CApplication::NotifyMouseUp()
{
	Palleon::CInputManager::SendInputEventToTree(m_uiViewport->GetSceneRoot(), m_mousePosition, Palleon::INPUT_EVENT_RELEASED);
	m_sceneCamera->NotifyMouseUp();
}

Palleon::CApplication* CreateApplication(bool)
{
	palleon_library_link();		//Needed for Android to work properly
	return new CApplication();
}
