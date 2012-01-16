#include "athena/NinePatchButton.h"

using namespace Athena;

CNinePatchButton::CNinePatchButton()
: m_releasedDescriptor(NULL)
, m_pressedDescriptor(NULL)
{
	m_background = CNinePatch::Create();
	m_background->GetMaterial()->SetIsTransparent(true);
	PrependChild(m_background);
}

CNinePatchButton::~CNinePatchButton()
{

}

NinePatchButtonPtr CNinePatchButton::Create()
{
	return NinePatchButtonPtr(new CNinePatchButton());
}

void CNinePatchButton::SetReleasedDescriptor(const CNinePatchDescriptor* descriptor)
{
	m_releasedDescriptor = descriptor;
	UpdateButtonState();
}

void CNinePatchButton::SetPressedDescriptor(const CNinePatchDescriptor* descriptor)
{
	m_pressedDescriptor = descriptor;
	UpdateButtonState();
}

void CNinePatchButton::SetSize(const CVector2& size)
{
	CButtonBase::SetSize(size);
    m_background->SetSize(size);
}

void CNinePatchButton::UpdateButtonState()
{
	switch(m_state)
	{
		case STATE_RELEASED:
			m_background->SetDescriptor(m_releasedDescriptor);
			break;
		case STATE_PRESSED:
			m_background->SetDescriptor(m_pressedDescriptor);
			break;
		default:
			assert(0);
			break;
	}
}
