#include "EmbedWindow.h"

#define WNDSTYLE		(WS_CLIPCHILDREN | WS_THICKFRAME | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX)

CEmbedWindow::CEmbedWindow()
{
	int logX = GetDeviceCaps(GetDC(NULL), LOGPIXELSX);
	int logY = GetDeviceCaps(GetDC(NULL), LOGPIXELSY);

	int windowWidth = MulDiv(800, logX, 96);
	int windowHeight = MulDiv(600, logY, 96);

	RECT windowRect;
	SetRect(&windowRect, 0, 0, windowWidth, windowHeight);
	AdjustWindowRectEx(&windowRect, WNDSTYLE, FALSE, NULL);

	Create(NULL, Framework::Win32::CDefaultWndClass::GetName(), _T("Embedding"), WNDSTYLE, windowRect, NULL, NULL);
	SetClassPtr();

	m_embedControl = new Palleon::CWin32EmbedControl(m_hWnd, Framework::Win32::CRect(0, 0, 32, 32),
		_T("ShadowMapping.exe"), _T("C:\\Projects\\Palleon\\samples\\ShadowMapping"));
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
