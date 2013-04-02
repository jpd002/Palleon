#include <assert.h>
#include "athena/win32/Win32ResourceManager.h"
#include "athena/win32/Win32AudioManager.h"
#include "athena/win32/Win32ApplicationWindow.h"
//#include "athena/win32/Dx9GraphicDevice.h"
#include "athena/win32/Dx11GraphicDevice.h"
#include "athena/ConfigManager.h"
#include "win32/Rect.h"
#include "string_cast.h"

#define CLSNAME			_T("MainWindow")
#define WNDSTYLE		(WS_CLIPCHILDREN | WS_DLGFRAME | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX)

#define PREFERENCE_SCREEN_WIDTH		("screen.width")
#define PREFERENCE_SCREEN_HEIGHT	("screen.height")
#define PREFERENCE_WINDOW_TITLE		("window.title")

using namespace Athena;

CWin32ApplicationWindow::CWin32ApplicationWindow()
: m_mouseX(0)
, m_mouseY(0)
, m_mouseDownPending(false)
, m_mouseUpPending(false)
, m_application(NULL)
, m_frameCounterTime(0)
, m_currentFrameCount(0)
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
	CConfigManager::GetInstance().GetConfig().RegisterPreferenceString(PREFERENCE_WINDOW_TITLE, "Athena Engine");
	CConfigManager::GetInstance().GetConfig().RegisterPreferenceInteger(PREFERENCE_SCREEN_WIDTH, 640);
	CConfigManager::GetInstance().GetConfig().RegisterPreferenceInteger(PREFERENCE_SCREEN_HEIGHT, 480);

	const char* windowTitle = CConfigManager::GetInstance().GetConfig().GetPreferenceString(PREFERENCE_WINDOW_TITLE);
	std::tstring windowTitleString = string_cast<std::tstring>(windowTitle);
	m_screenWidth = CConfigManager::GetInstance().GetConfig().GetPreferenceInteger(PREFERENCE_SCREEN_WIDTH);
	m_screenHeight = CConfigManager::GetInstance().GetConfig().GetPreferenceInteger(PREFERENCE_SCREEN_HEIGHT);

	QueryPerformanceFrequency(&m_counterFreq);
	QueryPerformanceCounter(&m_previousTime);

	RECT windowRect;
	SetRect(&windowRect, 0, 0, m_screenWidth, m_screenHeight);
	AdjustWindowRectEx(&windowRect, WNDSTYLE, FALSE, NULL);

	Create(NULL, CLSNAME, windowTitleString.c_str(), WNDSTYLE, &windowRect, NULL, NULL);
	SetClassPtr();

	CDx11GraphicDevice::CreateInstance(m_hWnd, CVector2(m_screenWidth, m_screenHeight));
	CWin32AudioManager::CreateInstance();

	m_application = CreateApplication();
}

CWin32ApplicationWindow::~CWin32ApplicationWindow()
{
	if(m_application != NULL)
	{
		delete m_application;
		m_application = NULL;
	}

	CWin32ResourceManager::DestroyInstance();
	CDx11GraphicDevice::DestroyInstance();
}

void CWin32ApplicationWindow::UpdateApplication()
{
	m_application->NotifyMouseMove(m_mouseX, m_mouseY);

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

	Athena::CDx11GraphicDevice::GetInstance().Draw();

	m_currentFrameCount++;
	if(m_frameCounterTime >= 1)
	{
		float frameRate = static_cast<float>(m_currentFrameCount) / m_frameCounterTime;
		m_frameCounterTime = 0;
		m_currentFrameCount = 0;
		static_cast<CDx11GraphicDevice&>(CDx11GraphicDevice::GetInstance()).SetFrameRate(frameRate);
	}
}

long CWin32ApplicationWindow::OnMouseMove(WPARAM param, int x, int y)
{
	m_mouseX = x;
	m_mouseY = y;
	return TRUE;
}

long CWin32ApplicationWindow::OnLeftButtonDown(int, int)
{
	m_mouseDownPending = true;
	return TRUE;
}

long CWin32ApplicationWindow::OnLeftButtonUp(int, int)
{
	m_mouseUpPending = true;
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

int WINAPI WinMain(HINSTANCE, HINSTANCE, char*, int)
{
	CWin32ApplicationWindow applicationWindow;
	applicationWindow.Center(NULL);
	applicationWindow.Show(SW_SHOW);
	applicationWindow.Loop();
	return 0;
}