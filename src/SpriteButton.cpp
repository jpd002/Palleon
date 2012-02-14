#include "athena/SpriteButton.h"

using namespace Athena;

CSpriteButton::CSpriteButton()
{
	m_background = CSprite::Create();
	m_background->GetMaterial()->SetIsTransparent(true);
	PrependChild(m_background);
}

CSpriteButton::~CSpriteButton()
{

}

SpriteButtonPtr CSpriteButton::Create()
{
	return std::make_shared<CSpriteButton>();
}

void CSpriteButton::SetReleasedTexture(const TexturePtr& texture)
{
	m_releasedTexture = texture;
	UpdateButtonState();
}

void CSpriteButton::SetPressedTexture(const TexturePtr& texture)
{
	m_pressedTexture = texture;
	UpdateButtonState();
}

void CSpriteButton::SetSize(const CVector2& size)
{
	CButtonBase::SetSize(size);
	m_background->SetSize(size);
}

void CSpriteButton::UpdateButtonState()
{
	switch(m_state)
	{
		case STATE_RELEASED:
			m_background->GetMaterial()->SetTexture(0, m_releasedTexture);
			break;
		case STATE_PRESSED:
			m_background->GetMaterial()->SetTexture(0, m_pressedTexture);
			break;
		default:
			assert(0);
			break;
	}
}
