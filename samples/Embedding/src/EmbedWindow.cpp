#include "EmbedWindow.h"

#define WNDSTYLE		(WS_CLIPCHILDREN | WS_THICKFRAME | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX)

CEmbedWindow::CEmbedWindow()
{
	RECT windowRect;
	SetRect(&windowRect, 0, 0, 800, 600);
	AdjustWindowRectEx(&windowRect, WNDSTYLE, FALSE, NULL);

	Create(NULL, Framework::Win32::CDefaultWndClass::GetName(), _T("Embedding"), WNDSTYLE, windowRect, NULL, NULL);
	SetClassPtr();

	m_embedControl = new Palleon::CWin32EmbedControl(m_hWnd, Framework::Win32::CRect(0, 0, 32, 32));
}

CEmbedWindow::~CEmbedWindow()
{
	delete m_embedControl;
}

long CEmbedWindow::OnSize(unsigned int, unsigned int, unsigned int)
{
	UpdateLayout();
	return FALSE;
}

void CEmbedWindow::UpdateLayout()
{
	auto clientRect = GetClientRect();
	clientRect.Inflate(-10, -10);
	m_embedControl->SetSizePosition(clientRect);
}
