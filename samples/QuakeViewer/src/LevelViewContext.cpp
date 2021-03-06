#include <assert.h>
#include <math.h>
#include "LevelViewContext.h"
#include "PtrStream.h"
#include "QuakeEntityParser.h"
#include "QuakeViewerDefs.h"
#include "string_format.h"

CLevelViewContext::CLevelViewContext(CPakFile* pakFile, const char* levelPath)
: CContextBase(pakFile)
, m_mousePosition(0, 0)
, m_commandMode(COMMAND_MODE_IDLE)
, m_cameraPosition(0, 0, 0)
, m_cameraHAngle(0)
, m_cameraVAngle(0)
, m_dragHAngle(0)
, m_dragVAngle(0)
, m_dragPosition(0, 0)
, m_bspFile(nullptr)
, m_elapsed(0)
{
	m_viewerPackage = Palleon::CPackage::Create("viewer");

	auto skyTexturePath = Palleon::CResourceManager::GetInstance().MakeResourcePath("viewer/skybox.dds");
	m_skyTexture = Palleon::CTextureLoader::CreateCubeTextureFromFile(skyTexturePath);

	{
		uint8* fileData = NULL;
		uint32 fileSize = 0;
		if(!m_pakFile->ReadFile(levelPath, &fileData, &fileSize))
		{
			throw std::runtime_error("Couldn't open file.");
		}
		Framework::CPtrStream inputStream(fileData, fileSize);
		m_bspFile = new CBspFile(inputStream);
		delete fileData;
	}

	{
		m_bspMapResourceProvider = new CBspMapResourceProvider();
		m_bspMapResourceProvider->LoadResources(*m_bspFile, *m_pakFile);
	}

	CVector3 originPosition(0, 0, 0);
	{
		QuakeEntityList entities = CQuakeEntityParser::Parse(m_bspFile->GetEntities());
		const QUAKE_ENTITY* info_player_entity = QuakeEntity::FindFirstEntityWithClassName(entities, "info_player_deathmatch");
		if(info_player_entity)
		{
			auto originValueIterator = info_player_entity->values.find("origin");
			if(originValueIterator != std::end(info_player_entity->values))
			{
				originPosition = QuakeEntity::ParseEntityVector(originValueIterator->second.c_str());
			}
		}
	}

	m_sphereBasePosition = originPosition;
	m_cameraPosition = originPosition + CVector3(0, 50, 0);

	InitializeReflectionMap();
	InitializeMapViewport();
	InitializeHudViewport();

	UpdateCamera();
}

CLevelViewContext::~CLevelViewContext()
{
	Palleon::CGraphicDevice::GetInstance().RemoveViewport(m_mapViewport.get());
	Palleon::CGraphicDevice::GetInstance().RemoveViewport(m_hudViewport.get());
	delete m_bspMapResourceProvider;
	delete m_bspFile;
}

void CLevelViewContext::InitializeMapViewport()
{
	m_bspMapMeshProvider = BspMapMeshProviderPtr(new CBspMapMeshProvider(m_bspFile, m_bspMapResourceProvider));

	m_mapViewport = Palleon::CViewport::Create();

	m_mapCamera = Palleon::CCamera::Create();
	CVector2 screenSize = Palleon::CGraphicDevice::GetInstance().GetScreenSize();
	m_mapCamera->SetPerspectiveProjection(M_PI / 4, screenSize.x / screenSize.y, 1, 10000);
	m_mapViewport->SetCamera(m_mapCamera);

	Palleon::CGraphicDevice::GetInstance().AddViewport(m_mapViewport.get());

	auto sceneRoot = m_mapViewport->GetSceneRoot();

	//Create skybox
	{
		auto skyBox = Palleon::CCubeMesh::Create();
		skyBox->SetIsPeggedToOrigin(true);
		skyBox->SetScale(CVector3(50, 50, 50));
		skyBox->GetMaterial()->SetCullingMode(Palleon::CULLING_CW);
		skyBox->GetMaterial()->SetTexture(0, m_skyTexture);
		skyBox->GetMaterial()->SetTextureCoordSource(0, Palleon::TEXTURE_COORD_CUBE_POS);
		sceneRoot->AppendChild(skyBox);
	}

	sceneRoot->AppendChild(m_bspMapMeshProvider);

	//Create reflective sphere
	{
		auto sphere = Palleon::CSphereMesh::Create();
		sphere->SetPosition(m_sphereBasePosition);
		sphere->SetScale(CVector3(25, 25, 25));
		sphere->GetMaterial()->SetTexture(0, m_reflectionRenderTarget);
		sphere->GetMaterial()->SetTextureCoordSource(0, Palleon::TEXTURE_COORD_CUBE_REFLECT);
		sceneRoot->AppendChild(sphere);
		m_sphere = sphere;
	}
}

void CLevelViewContext::InitializeHudViewport()
{
	CVector2 screenSize = Palleon::CGraphicDevice::GetInstance().GetScreenSize();

	m_hudViewport = Palleon::CViewport::Create();

	{
		auto camera = Palleon::CCamera::Create();
		camera->SetupOrthoCamera(screenSize.x, screenSize.y);
		m_hudViewport->SetCamera(camera);
	}
	Palleon::CGraphicDevice::GetInstance().AddViewport(m_hudViewport.get());

	//Create hud elements
	{
		auto sceneRoot = m_hudViewport->GetSceneRoot();

		{
			auto scene = Palleon::CScene::Create(Palleon::CResourceManager::GetInstance().GetResource<Palleon::CSceneDescriptor>("levelviewhud_scene.xml"));

			{
				auto sceneLayout = scene->GetLayout();
				sceneLayout->SetRect(10, 10, screenSize.x - 10, screenSize.y - 10);
				sceneLayout->RefreshGeometry();
			}

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

			m_positionLabel = scene->FindNode<Palleon::CLabel>("PositionLabel");
			m_metricsLabel = scene->FindNode<Palleon::CLabel>("MetricsLabel");

			sceneRoot->AppendChild(scene);
		}
	}
}

void CLevelViewContext::InitializeReflectionMap()
{
	m_reflectionCamera = Palleon::CCamera::Create();
	m_reflectionCamera->SetPerspectiveProjection(M_PI / 2, 1, 1, 5000);

	m_reflectionRenderTarget = Palleon::CGraphicDevice::GetInstance().CreateCubeRenderTarget(Palleon::TEXTURE_FORMAT_RGB888, 128);
}

void CLevelViewContext::Update(float dt)
{
	m_sphere->SetPosition(m_sphereBasePosition + CVector3(0, 150 * (sin(m_elapsed) / 2.f + 0.5f), 0));
//	m_sphere->SetPosition(m_cameraPosition + CVector3(150 * sin(m_elapsed), 0, 150 * cos(m_elapsed)));

	m_elapsed += dt;

	if(m_commandMode == COMMAND_MODE_DRAG_CAMERA)
	{
		float deltaX = m_dragPosition.x - m_mousePosition.x;
		float deltaY = m_dragPosition.y - m_mousePosition.y;
		m_cameraHAngle = m_dragHAngle - deltaX * 0.015f;
		m_cameraVAngle = m_dragVAngle - deltaY * 0.015f;
		m_cameraVAngle = std::min<float>(m_cameraVAngle, M_PI / 2);
		m_cameraVAngle = std::max<float>(m_cameraVAngle, -M_PI / 2);
	}
	else if(m_commandMode == COMMAND_MODE_MOVE_FORWARD || m_commandMode == COMMAND_MODE_MOVE_BACKWARD)
	{
		CMatrix4 yawMatrix(CMatrix4::MakeAxisYRotation(m_cameraHAngle));
		CMatrix4 pitchMatrix(CMatrix4::MakeAxisXRotation(m_cameraVAngle));
		CMatrix4 totalMatrix = yawMatrix * pitchMatrix;
		CVector3 forwardVector = CVector3(0, 0, 1) * totalMatrix;
		float direction = (m_commandMode == COMMAND_MODE_MOVE_BACKWARD) ? (-1.0f) : (1.0f);
		m_cameraPosition += (forwardVector * direction * 400.f * dt);
	}

	UpdateCamera();

	{
		auto positionText = string_format("Pos = (X: %0.2f, Y: %0.2f, Z: %0.2f)", m_cameraPosition.x, m_cameraPosition.y, m_cameraPosition.z);
		m_positionLabel->SetText(positionText);
	}

	{
		auto metricsText = string_format("Draw Calls = %d - FPS = %d", 
			Palleon::CGraphicDevice::GetInstance().GetDrawCallCount(),
			static_cast<int>(Palleon::CGraphicDevice::GetInstance().GetFrameRate()));
		m_metricsLabel->SetText(metricsText);
	}

	m_bspMapResourceProvider->UpdateMaterials(dt);

	m_mapViewport->GetSceneRoot()->Update(dt);
	m_mapViewport->GetSceneRoot()->UpdateTransformations();
	m_hudViewport->GetSceneRoot()->Update(dt);
	m_hudViewport->GetSceneRoot()->UpdateTransformations();

	UpdateReflectionMap();
}

void CLevelViewContext::UpdateCamera()
{
	CMatrix4 yawMatrix(CMatrix4::MakeAxisYRotation(m_cameraHAngle));
	CMatrix4 pitchMatrix(CMatrix4::MakeAxisXRotation(m_cameraVAngle));
	CMatrix4 translationMatrix(CMatrix4::MakeTranslation(-m_cameraPosition.x, -m_cameraPosition.y, -m_cameraPosition.z));

	CMatrix4 rotationMatrix = yawMatrix * pitchMatrix;
	CMatrix4 totalMatrix = translationMatrix * rotationMatrix;

	m_mapCamera->SetViewMatrix(totalMatrix);
}

void CLevelViewContext::UpdateReflectionMap()
{
	CVector3 spherePosition = m_sphere->GetPosition();
	m_sphere->SetVisible(false);

	static const CVector3 lookAtDirections[6] =
	{
		CVector3(100, 0, 0),
		CVector3(-100, 0, 0),
		CVector3(0, 100, 0),
		CVector3(0, -100, 0),
		CVector3(0, 0, 100),
		CVector3(0, 0, -100)
	};

	static const CVector3 lookAtUps[6] =
	{
		CVector3(0, 1, 0),
		CVector3(0, 1, 0),
		CVector3(0, 0, -1),
		CVector3(0, 0, 1),
		CVector3(0, 1, 0),
		CVector3(0, 1, 0)
	};

	for(unsigned int i = 0; i < 6; i++)
	{
		m_reflectionCamera->LookAt(m_sphere->GetPosition(), spherePosition + lookAtDirections[i], lookAtUps[i]);
		m_mapViewport->SetCamera(m_reflectionCamera);
		//m_reflectionRenderTarget->Draw(static_cast<Palleon::CUBEMAP_FACE>(i), m_mapViewport);
	}

	m_mapViewport->SetCamera(m_mapCamera);
	m_sphere->SetVisible(true);
}

void CLevelViewContext::NotifyMouseMove(int x, int y)
{
	m_mousePosition = CVector2(x, y);
}

void CLevelViewContext::NotifyMouseDown()
{
	if(m_forwardButtonBoundingBox.Intersects(CBox2(m_mousePosition.x, m_mousePosition.y, 4, 4)))
	{
		m_commandMode = COMMAND_MODE_MOVE_FORWARD;
	}
	else if(m_backwardButtonBoundingBox.Intersects(CBox2(m_mousePosition.x, m_mousePosition.y, 4, 4)))
	{
		m_commandMode = COMMAND_MODE_MOVE_BACKWARD;
	}
	else
	{
		m_commandMode = COMMAND_MODE_DRAG_CAMERA;
		m_dragHAngle = m_cameraHAngle;
		m_dragVAngle = m_cameraVAngle;
		m_dragPosition = m_mousePosition;
	}
}

void CLevelViewContext::NotifyMouseUp()
{
	m_commandMode = COMMAND_MODE_IDLE;
}
