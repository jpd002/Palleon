#include "athena/Button.h"

using namespace Athena;

CButton::CButton()
: m_state(STATE_RELEASED)
{
    m_background = CSprite::Create();
    m_background->GetMaterial()->SetIsTransparent(true);
    
    m_label = CLabel::Create();
    m_label->SetHorizontalAlignment(CLabel::HORIZONTAL_ALIGNMENT_CENTER);
    m_label->SetVerticalAlignment(CLabel::VERTICAL_ALIGNMENT_CENTER);
    
    AppendChild(m_background);
    AppendChild(m_label);
}

CButton::~CButton()
{
    
}

ButtonPtr CButton::Create()
{
    return ButtonPtr(new CButton());
}

void CButton::SetText(const char* caption)
{
    m_label->SetText(caption);
}

void CButton::SetFont(const CFontDescriptor* descriptor)
{
    m_label->SetFont(descriptor);
}

void CButton::SetReleasedTexture(const TexturePtr& texture)
{
    m_releasedTexture = texture;
    UpdateButtonState();
}

void CButton::SetPressedTexture(const TexturePtr& texture)
{
    m_pressedTexture = texture;
    UpdateButtonState();
}

void CButton::SetTextureExtends(float x, float y)
{
    m_background->SetTextureExtends(x, y);
}

void CButton::SetSize(const CVector2& size)
{
    CWidget::SetSize(size);
    m_background->SetSize(size);
    m_label->SetSize(size);
}

void CButton::UpdateButtonState()
{
    switch(m_state)
    {
        case STATE_PRESSED:
            m_background->GetMaterial()->SetTexture(0, m_pressedTexture);
            break;
        case STATE_RELEASED:
            m_background->GetMaterial()->SetTexture(0, m_releasedTexture);
            break;
        default:
            assert(0);
            break;
    }
}

void CButton::OnTouchPressed(bool inside)
{
    if(inside)
    {
        m_state = STATE_PRESSED;
        UpdateButtonState();
    }
}

void CButton::OnTouchReleased(bool inside)
{
    if(inside && m_state == STATE_PRESSED)
    {
        Press();
    }
    m_state = STATE_RELEASED;
    UpdateButtonState();    
}
