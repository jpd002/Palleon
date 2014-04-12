#include "palleon/ButtonBase.h"

using namespace Palleon;

CButtonBase::CButtonBase()
: m_state(STATE_RELEASED)
{
	m_label = CLabel::Create();
	m_label->SetHorizontalAlignment(CLabel::HORIZONTAL_ALIGNMENT_CENTER);
	m_label->SetVerticalAlignment(CLabel::VERTICAL_ALIGNMENT_CENTER);

	AppendChild(m_label);
}

CButtonBase::~CButtonBase()
{

}

void CButtonBase::SetText(const std::string& caption)
{
	m_label->SetText(caption);
}

void CButtonBase::SetFont(const CFontDescriptor* descriptor)
{
	m_label->SetFont(descriptor);
}

void CButtonBase::SetSize(const CVector2& size)
{
	CWidget::SetSize(size);
	m_label->SetSize(size);
}

void CButtonBase::OnTouchPressed(bool inside)
{
	if(inside)
	{
		m_state = STATE_PRESSED;
		UpdateButtonState();
	}
}

void CButtonBase::OnTouchReleased(bool inside)
{
	if(inside && m_state == STATE_PRESSED)
	{
		Press(this);
	}
	m_state = STATE_RELEASED;
	UpdateButtonState();
}
