#include <assert.h>
#include "athena/win32/Win32ResourceManager.h"
#include "athena/win32/Dx9GraphicDevice.h"
#include "athena/win32/Dx9ApplicationWindow.h"
#include "athena/ConfigManager.h"
#include "win32/Rect.h"
#include "string_cast.h"

#define CLSNAME			_T("MainWindow")
#define WNDSTYLE		(WS_CLIPCHILDREN | WS_DLGFRAME | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX)

#define PREFERENCE_SCREEN_WIDTH		("screen.width")
#define PREFERENCE_SCREEN_HEIGHT	("screen.height")
#define PREFERENCE_WINDOW_TITLE		("window.title")

using namespace Athena;

CDx9ApplicationWindow::CDx9ApplicationWindow()
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

	CDx9GraphicDevice::CreateInstance(m_hWnd, CVector2(m_screenWidth, m_screenHeight));
	CWin32ResourceManager::CreateInstance(static_cast<CDx9GraphicDevice&>(CDx9GraphicDevice::GetInstance()).GetDevice());

	m_application = CreateApplication();
}

CDx9ApplicationWindow::~CDx9ApplicationWindow()
{
	if(m_application != NULL)
	{
		delete m_application;
		m_application = NULL;
	}

	CWin32ResourceManager::DestroyInstance();
	CDx9GraphicDevice::DestroyInstance();
}

void CDx9ApplicationWindow::UpdateApplication()
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

	Athena::CDx9GraphicDevice::GetInstance().Draw();

	m_currentFrameCount++;
	if(m_frameCounterTime >= 1)
	{
		float frameRate = static_cast<float>(m_currentFrameCount) / m_frameCounterTime;
		m_frameCounterTime = 0;
		m_currentFrameCount = 0;
		static_cast<CDx9GraphicDevice&>(CDx9GraphicDevice::GetInstance()).SetFrameRate(frameRate);
	}
}

long CDx9ApplicationWindow::OnMouseMove(WPARAM param, int x, int y)
{
	m_mouseX = x;
	m_mouseY = y;
	return TRUE;
}

long CDx9ApplicationWindow::OnLeftButtonDown(int, int)
{
	m_mouseDownPending = true;
	return TRUE;
}

long CDx9ApplicationWindow::OnLeftButtonUp(int, int)
{
	m_mouseUpPending = true;
	return TRUE;
}

void CDx9ApplicationWindow::Loop()
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
	CDx9ApplicationWindow applicationWindow;
	applicationWindow.Center(NULL);
	applicationWindow.Show(SW_SHOW);
	applicationWindow.Loop();
	return 0;
}
