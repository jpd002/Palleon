#include <assert.h>
#include "palleon/win32/Win32ResourceManager.h"
#include "palleon/win32/Win32AudioManager.h"
#include "palleon/win32/Win32ApplicationWindow.h"
#include "palleon/win32/Dx11GraphicDevice.h"
#include "palleon/ConfigManager.h"
#include "win32/Rect.h"
#include "string_cast.h"

#define CLSNAME			_T("MainWindow")
#define WNDSTYLE		(WS_CLIPCHILDREN | WS_DLGFRAME | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX)

#define PREFERENCE_SCREEN_WIDTH		("screen.width")
#define PREFERENCE_SCREEN_HEIGHT	("screen.height")
#define PREFERENCE_WINDOW_TITLE		("window.title")

using namespace Palleon;

CWin32ApplicationWindow::CWin32ApplicationWindow()
{
	if(!DoesWindowClassExist(CLSNAME))
	{
		WNDCLASSEX w;
		memset(&w, 0, sizeof(WNDCLASSEX));
		w.cbSize		= sizeof(WNDCLASSEX);
		w.lpfnWndProc	= CWindow::WndProc;
		w.lpszClassName	= CLSNAME;
		w.hbrBackground	= NULL;
		w.hInstance		= GetModuleHandle(NULL);
		w.hCursor		= LoadCursor(NULL, IDC_ARROW);
		RegisterClassEx(&w);
	}

	CWin32ResourceManager::CreateInstance();

	CConfigManager::CreateInstance();
	CConfigManager::GetInstance().GetConfig().RegisterPreferenceString(PREFERENCE_WINDOW_TITLE, "Palleon Engine");
	CConfigManager::GetInstance().GetConfig().RegisterPreferenceInteger(PREFERENCE_SCREEN_WIDTH, 640);
	CConfigManager::GetInstance().GetConfig().RegisterPreferenceInteger(PREFERENCE_SCREEN_HEIGHT, 480);

	const char* windowTitle = CConfigManager::GetInstance().GetConfig().GetPreferenceString(PREFERENCE_WINDOW_TITLE);
	std::tstring windowTitleString = string_cast<std::tstring>(windowTitle);
	CVector2 screenSize = 
		CVector2
		(
			CConfigManager::GetInstance().GetConfig().GetPreferenceInteger(PREFERENCE_SCREEN_WIDTH),
			CConfigManager::GetInstance().GetConfig().GetPreferenceInteger(PREFERENCE_SCREEN_HEIGHT)
		);

	QueryPerformanceFrequency(&m_counterFreq);
	QueryPerformanceCounter(&m_previousTime);

	int logX = GetDeviceCaps(GetDC(NULL), LOGPIXELSX);
	int logY = GetDeviceCaps(GetDC(NULL), LOGPIXELSY);
	m_dpiScale.x = static_cast<float>(logX) / 96.f;
	m_dpiScale.y = static_cast<float>(logY) / 96.f;

	CVector2 realScreenSize(screenSize * m_dpiScale);

	RECT windowRect;
	SetRect(&windowRect, 0, 0, realScreenSize.x, realScreenSize.y);
	AdjustWindowRectEx(&windowRect, WNDSTYLE, FALSE, NULL);

	Create(NULL, CLSNAME, windowTitleString.c_str(), WNDSTYLE, windowRect, NULL, NULL);
	SetClassPtr();

	CDx11GraphicDevice::CreateInstance(m_hWnd, screenSize, realScreenSize);
	CWin32AudioManager::CreateInstance();

	m_application = CreateApplication();
}

CWin32ApplicationWindow::~CWin32ApplicationWindow()
{
	delete m_application;
	CWin32ResourceManager::DestroyInstance();
	CDx11GraphicDevice::DestroyInstance();
}

void CWin32ApplicationWindow::UpdateApplication()
{
	m_application->NotifyMouseMove(m_mouseX, m_mouseY);

	if(m_mouseZ != 0)
	{
		m_application->NotifyMouseWheel(m_mouseZ);
		m_mouseZ = 0;
	}

	if(m_mouseDownPending)
	{
		m_application->NotifyMouseDown();
		m_mouseDownPending = false;
	}

	if(m_mouseUpPending)
	{
		m_application->NotifyMouseUp();
		m_mouseUpPending = false;
	}

	LARGE_INTEGER currentTime, deltaTimeAbs;
	QueryPerformanceCounter(&currentTime);

	deltaTimeAbs.QuadPart = (currentTime.QuadPart - m_previousTime.QuadPart);
	float deltaTimeRel = static_cast<float>(deltaTimeAbs.QuadPart) / static_cast<float>(m_counterFreq.QuadPart);
	m_frameCounterTime += deltaTimeRel;
	if(deltaTimeRel > 1)
	{
		deltaTimeRel = 1;
	}
	m_previousTime = currentTime;

	m_application->Update(deltaTimeRel);

	Palleon::CGraphicDevice::GetInstance().Draw();

	m_currentFrameCount++;
	if(m_frameCounterTime >= 1)
	{
		float frameRate = static_cast<float>(m_currentFrameCount) / m_frameCounterTime;
		m_frameCounterTime = 0;
		m_currentFrameCount = 0;
		static_cast<CDx11GraphicDevice&>(CGraphicDevice::GetInstance()).SetFrameRate(frameRate);
	}
}

KEY_CODE CWin32ApplicationWindow::TranslateKeyCode(unsigned int keyCode)
{
	switch(keyCode)
	{
	case 'W':		return KEY_CODE_W;
	case 'A':		return KEY_CODE_A;
	case 'S':		return KEY_CODE_S;
	case 'D':		return KEY_CODE_D;
	case VK_DOWN:	return KEY_CODE_ARROW_DOWN;
	case VK_UP:		return KEY_CODE_ARROW_UP;
	case VK_LEFT:	return KEY_CODE_ARROW_LEFT;
	case VK_RIGHT:	return KEY_CODE_ARROW_RIGHT;
	default:		return KEY_CODE_NONE;
	}
}

long CWin32ApplicationWindow::OnActivate(unsigned int activeState, bool, HWND)
{
	if(activeState == WA_INACTIVE)
	{
		m_application->NotifyInputCancelled();
		m_mouseDownPending = false;
		m_mouseUpPending = false;
	}
	return FALSE;
}

long CWin32ApplicationWindow::OnSize(unsigned int, unsigned int x, unsigned int y)
{
	CVector2 realScreenSize(x, y);
	CVector2 screenSize(realScreenSize / m_dpiScale);
	static_cast<CDx11GraphicDevice&>(CGraphicDevice::GetInstance()).SetOutputBufferSize(screenSize, realScreenSize);
	m_application->NotifySizeChanged();
	return FALSE;
}

long CWin32ApplicationWindow::OnMouseMove(WPARAM param, int x, int y)
{
	m_mouseX = static_cast<int>(static_cast<float>(x) / m_dpiScale.x);
	m_mouseY = static_cast<int>(static_cast<float>(y) / m_dpiScale.y);
	return TRUE;
}

long CWin32ApplicationWindow::OnMouseWheel(int x, int y, short z)
{
	m_mouseZ += z;
	return TRUE;
}

long CWin32ApplicationWindow::OnMouseLeave()
{
	m_application->NotifyInputCancelled();
	m_mouseDownPending = false;
	m_mouseUpPending = false;
	m_mouseTrackingEnabled = false;
	return TRUE;
}

long CWin32ApplicationWindow::OnLeftButtonDown(int, int)
{
	SetCapture(m_hWnd);
	m_mouseDownPending = true;
	return TRUE;
}

long CWin32ApplicationWindow::OnLeftButtonUp(int, int)
{
	ReleaseCapture();
	m_mouseUpPending = true;
	return TRUE;
}

long CWin32ApplicationWindow::OnKeyDown(WPARAM keyCode, LPARAM flags)
{
	//Check repeat flag
	if((HIWORD(flags) & KF_REPEAT) == 0)
	{
		auto translatedKeyCode = TranslateKeyCode(keyCode);
		if(translatedKeyCode != KEY_CODE_NONE)
		{
			m_application->NotifyKeyDown(translatedKeyCode);
		}
	}
	return TRUE;
}

long CWin32ApplicationWindow::OnKeyUp(WPARAM keyCode, LPARAM flags)
{
	auto translatedKeyCode = TranslateKeyCode(keyCode);
	if(translatedKeyCode != KEY_CODE_NONE)
	{
		m_application->NotifyKeyUp(translatedKeyCode);
	}
	return TRUE;
}

void CWin32ApplicationWindow::Loop()
{
	while(IsWindow())
	{
		MSG m;
		while(PeekMessage(&m, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&m);
			DispatchMessage(&m);
		}
		UpdateApplication();
	}
}
