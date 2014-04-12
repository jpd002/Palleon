#include "LevelSelectionContext.h"
#include "QuakeViewerDefs.h"

const char* CLevelSelectionContext::g_levelPaths[MAX_LEVELS] =
{
	"maps/q3dm1.bsp",
	"maps/q3dm7.bsp",
	"maps/q3dm17.bsp",
	"maps/q3tourney2.bsp"
};

const char* CLevelSelectionContext::g_levelShotTexturePaths[MAX_LEVELS] =
{
	"levelshots/Q3DM1.jpg",
	"levelshots/Q3DM7.jpg",
	"levelshots/Q3DM17.jpg",
	"levelshots/Q3TOURNEY2.jpg"
};

#define SHOT_SIZE		128
#define MARGIN_SIZE		16

CLevelSelectionContext::CLevelSelectionContext(CPakFile* pakFile)
: CContextBase(pakFile)
, m_mousePosition(0, 0)
{
	CVector2 screenSize = Palleon::CGraphicDevice::GetInstance().GetScreenSize();

	m_viewport = Palleon::CViewport::Create();

	{
		Palleon::CameraPtr camera = Palleon::CCamera::Create();
		camera->SetupOrthoCamera(screenSize.x, screenSize.y);
		m_viewport->SetCamera(camera);
	}
	Palleon::CGraphicDevice::GetInstance().AddViewport(m_viewport.get());
	
	for(unsigned int i = 0; i < MAX_LEVELS; i++)
	{
		m_levelShotTextures[i] = LoadTexture(g_levelShotTexturePaths[i]);
	}

	{
		Palleon::SceneNodePtr sceneRoot = m_viewport->GetSceneRoot();

		{
			Palleon::LabelPtr label = Palleon::CLabel::Create();
			label->SetFont(Palleon::CResourceManager::GetInstance().GetResource<Palleon::CFontDescriptor>(FONTDESCRIPTOR_NAME_DEFAULT));
			label->SetPosition(CVector3(0, 0, 0));
			label->SetSize(screenSize);
			label->SetHorizontalAlignment(Palleon::CLabel::HORIZONTAL_ALIGNMENT_CENTER);
			label->SetText("Select your destiny");
			label->SetTextScale(CVector2(0.75, 0.75));
			sceneRoot->AppendChild(label);
		}

		float posX = (screenSize.x - (2 * SHOT_SIZE + MARGIN_SIZE)) / 2;
		float posY = 32;

		for(unsigned int i = 0; i < MAX_LEVELS; i++)
		{
			unsigned int x = i % 2;
			unsigned int y = i / 2;
			Palleon::SpriteButtonPtr button = Palleon::CSpriteButton::Create();
			button->SetPosition(CVector3(
				posX + (x * (SHOT_SIZE + MARGIN_SIZE)), 
				posY + (y * (SHOT_SIZE + MARGIN_SIZE)), 0));
			button->SetSize(CVector2(128, 128));
			button->SetReleasedTexture(m_levelShotTextures[i]);
			button->SetPressedTexture(m_levelShotTextures[i]);
			button->Press.connect(boost::bind(&CLevelSelectionContext::OnShotButtonPress, this, i));
			sceneRoot->AppendChild(button);
		}
	}
}

CLevelSelectionContext::~CLevelSelectionContext()
{
	Palleon::CGraphicDevice::GetInstance().RemoveViewport(m_viewport.get());
}

void CLevelSelectionContext::Update(float dt)
{
	m_viewport->GetSceneRoot()->Update(dt);
	m_viewport->GetSceneRoot()->UpdateTransformations();
}

void CLevelSelectionContext::NotifyMouseMove(int x, int y)
{
	m_mousePosition = CVector2(x, y);
}

void CLevelSelectionContext::NotifyMouseDown()
{
	Palleon::CInputManager::SendInputEventToTree(m_viewport->GetSceneRoot(), m_mousePosition, Palleon::INPUT_EVENT_PRESSED);
}

void CLevelSelectionContext::NotifyMouseUp()
{
	Palleon::CInputManager::SendInputEventToTree(m_viewport->GetSceneRoot(), m_mousePosition, Palleon::INPUT_EVENT_RELEASED);
}

Palleon::TexturePtr CLevelSelectionContext::LoadTexture(const char* texturePath)
{
	uint8* fileData(NULL);
	uint32 fileSize(0);
	if(!m_pakFile->ReadFile(texturePath, &fileData, &fileSize))
	{
		return Palleon::TexturePtr();
	}

	auto result = Palleon::CTextureLoader::CreateTextureFromMemory(fileData, fileSize);
	delete fileData;

	return result;
}

void CLevelSelectionContext::OnShotButtonPress(unsigned int shotIndex)
{
	Finished(QuakeViewer::CONTEXT_LEVEL_VIEW, g_levelPaths[shotIndex]);
}
