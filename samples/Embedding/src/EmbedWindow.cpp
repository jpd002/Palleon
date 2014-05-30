#include "EmbedWindow.h"

#define WNDSTYLE		(WS_CLIPCHILDREN | WS_DLGFRAME | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX)

CEmbedWindow::CEmbedWindow()
{
	RECT windowRect;
	SetRect(&windowRect, 0, 0, 800, 600);
	AdjustWindowRectEx(&windowRect, WNDSTYLE, FALSE, NULL);

	Create(NULL, Framework::Win32::CDefaultWndClass::GetName(), _T("Embedding"), WNDSTYLE, windowRect, NULL, NULL);
	SetClassPtr();

	m_embedControl = new Palleon::CWin32EmbedControl(m_hWnd, Framework::Win32::CRect(80, 60, 720, 540));
}

CEmbedWindow::~CEmbedWindow()
{
	delete m_embedControl;
}
