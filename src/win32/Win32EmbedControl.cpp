#include <stdexcept>
#include "palleon/win32/Win32EmbedControl.h"
#include "string_cast.h"

using namespace Palleon;

#define CLSNAME							_T("Win32EmbedControl")
#define WNDSTYLE						(WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_TABSTOP)
#define WNDSTYLEEX						(0)

#define TIMER_DELAY						16
#define TIMER_ID						1

CWin32EmbedControl::CWin32EmbedControl(HWND parentWnd, const RECT& windowRect,
	const std::tstring& executablePath, const std::tstring& workingDir)
: m_embedClient(executablePath, workingDir)
{
	if(!DoesWindowClassExist(CLSNAME))
	{
		WNDCLASSEX w;
		memset(&w, 0, sizeof(WNDCLASSEX));
		w.cbSize		= sizeof(WNDCLASSEX);
		w.lpfnWndProc	= CWindow::WndProc;
		w.lpszClassName	= CLSNAME;
		w.hbrBackground = NULL;
		w.hInstance		= GetModuleHandle(NULL);
		w.hCursor		= LoadCursor(NULL, IDC_ARROW);
		RegisterClassEx(&w);
	}

	Create(WNDSTYLEEX, CLSNAME, _T(""), WNDSTYLE, windowRect, parentWnd, this);
	SetClassPtr();

	CreateDevice();
	CreateSharedTexture();

	SetRunning(true);
}

CWin32EmbedControl::~CWin32EmbedControl()
{

}

void CWin32EmbedControl::SetRunning(bool running)
{
	if(!m_running && running)
	{
		SetTimer(m_hWnd, TIMER_ID, TIMER_DELAY, 0);
		m_running = true;
		return;
	}
	if(m_running && !running)
	{
		KillTimer(m_hWnd, TIMER_ID);
		m_running = false;
		return;
	}
}

bool CWin32EmbedControl::IsClientActive() const
{
	return m_embedClientActive;
}

std::string CWin32EmbedControl::ExecuteCommand(const std::string& command)
{
	std::string commandResult;
	if(m_embedClientActive)
	{
		//Disable timer to that we don't get in a situation where
		//the control is locked because lots of WM_TIMER messages are sent
		bool runningBefore = m_running;
		SetRunning(false);

		BSTR rpcCommandResult = nullptr;
		BSTR rpcCommand = SysAllocString(string_cast<std::wstring>(command).c_str());

		auto application = m_embedClient.GetApplication();
		HRESULT result = application->NotifyExternalCommand(rpcCommand, &rpcCommandResult);

		SysFreeString(rpcCommand);
		if(rpcCommandResult != nullptr)
		{
			commandResult = string_cast<std::string>(rpcCommandResult);
			SysFreeString(rpcCommandResult);
		}

		//Enable timer here because "ErrorRaised" might change
		//the running state for some reason
		SetRunning(runningBefore);

		if(FAILED(result))
		{
			m_embedClientActive = false;
			ErrorRaised(this);
		}
	}
	return commandResult;
}

void CWin32EmbedControl::CreateDevice()
{
	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};

	HRESULT result = S_OK;

	auto clientRect = GetSurfaceSize();

	swapChainDesc.BufferCount							= 1;
	swapChainDesc.BufferDesc.Width						= clientRect.Width();
	swapChainDesc.BufferDesc.Height						= clientRect.Height();
	swapChainDesc.BufferDesc.Format						= DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.RefreshRate.Numerator		= 0;
	swapChainDesc.BufferDesc.RefreshRate.Denominator	= 1;
	swapChainDesc.BufferDesc.ScanlineOrdering			= DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling					= DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.BufferUsage							= DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow							= m_hWnd;
	swapChainDesc.SampleDesc.Count						= 1;
	swapChainDesc.SampleDesc.Quality					= 0;
	swapChainDesc.Windowed								= true;
	swapChainDesc.SwapEffect							= DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Flags									= 0;

	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

	UINT deviceCreationFlags = 0;
#ifdef _DEBUG
	deviceCreationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	result = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, deviceCreationFlags, &featureLevel, 1, D3D11_SDK_VERSION, 
		&swapChainDesc, &m_swapChain, &m_device, NULL, &m_deviceContext);
	assert(SUCCEEDED(result));

	CreateOutputTexture();
}

void CWin32EmbedControl::CreateOutputTexture()
{
	HRESULT result = S_OK;

	{
		result = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&m_outputTexture));
		assert(SUCCEEDED(result));
	
		result = m_device->CreateRenderTargetView(m_outputTexture, nullptr, &m_outputTextureView);
		assert(SUCCEEDED(result));
	}
}

void CWin32EmbedControl::CreateSharedTexture()
{
	if(!m_embedClientActive)
	{
		return;
	}

	HRESULT result = S_OK;
	auto surfaceSize = GetSurfaceSize();

	auto application = m_embedClient.GetApplication();

	result = application->SetSurfaceSize(surfaceSize.Width(), surfaceSize.Height());
	assert(SUCCEEDED(result));
	if(FAILED(result))
	{
		m_embedClientActive = false;
		ErrorRaised(this);
		return;
	}

	HANDLE surfaceHandle = INVALID_HANDLE_VALUE;
	result = application->GetSurfaceHandle(reinterpret_cast<DWORD_PTR*>(&surfaceHandle));
	assert(SUCCEEDED(result));
	if(FAILED(result))
	{
		m_embedClientActive = false;
		ErrorRaised(this);
		return;
	}

	result = m_device->OpenSharedResource(surfaceHandle, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&m_sharedTexture));
	assert(SUCCEEDED(result));

	result = m_sharedTexture->QueryInterface<IDXGIKeyedMutex>(reinterpret_cast<IDXGIKeyedMutex**>(&m_sharedTextureMutex));
	assert(SUCCEEDED(result));
}

Framework::Win32::CRect CWin32EmbedControl::GetSurfaceSize()
{
	auto clientRect = GetClientRect();
	int widthAdjust = 0, heightAdjust = 0;
	if(clientRect.Width() < 8)
	{
		widthAdjust = 8 - clientRect.Width();
	}
	if(clientRect.Height() < 8)
	{
		heightAdjust = 8 - clientRect.Height();
	}
	clientRect.Inflate(widthAdjust, heightAdjust);
	return clientRect;
}

KEY_CODE CWin32EmbedControl::TranslateKeyCode(unsigned int keyCode)
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

long CWin32EmbedControl::OnMouseMove(WPARAM, int posX, int posY)
{
	if(m_embedClientActive)
	{
		auto application = m_embedClient.GetApplication();
		HRESULT result = application->NotifyMouseMove(posX, posY);
		assert(SUCCEEDED(result));
		if(FAILED(result))
		{
			m_embedClientActive = false;
			ErrorRaised(this);
		}
	}
	return FALSE;
}

long CWin32EmbedControl::OnMouseWheel(int, int, short z)
{
	if(m_embedClientActive)
	{
		auto application = m_embedClient.GetApplication();
		HRESULT result = application->NotifyMouseWheel(z);
		assert(SUCCEEDED(result));
		if(FAILED(result))
		{
			m_embedClientActive = false;
			ErrorRaised(this);
		}
	}
	return FALSE;
}

long CWin32EmbedControl::OnLeftButtonDown(int, int)
{
	SetFocus();
	SetCapture(m_hWnd);
	if(m_embedClientActive)
	{
		auto application = m_embedClient.GetApplication();
		HRESULT result = application->NotifyMouseDown();
		assert(SUCCEEDED(result));
		if(FAILED(result))
		{
			m_embedClientActive = false;
			ErrorRaised(this);
		}
	}
	return FALSE;
}

long CWin32EmbedControl::OnLeftButtonUp(int, int)
{
	ReleaseCapture();
	if(m_embedClientActive)
	{
		auto application = m_embedClient.GetApplication();
		HRESULT result = application->NotifyMouseUp();
		assert(SUCCEEDED(result));
		if(FAILED(result))
		{
			m_embedClientActive = false;
			ErrorRaised(this);
		}
	}
	return FALSE;
}

long CWin32EmbedControl::OnKeyDown(WPARAM keyCode, LPARAM flags)
{
	//Check repeat flag
	if((flags & 0x40000000) == 0)
	{
		auto translatedKeyCode = TranslateKeyCode(keyCode);
		if(translatedKeyCode != KEY_CODE_NONE)
		{
			auto application = m_embedClient.GetApplication();
			application->NotifyKeyDown(translatedKeyCode);
		}
	}
	return TRUE;
}

long CWin32EmbedControl::OnKeyUp(WPARAM keyCode, LPARAM flags)
{
	auto translatedKeyCode = TranslateKeyCode(keyCode);
	if(translatedKeyCode != KEY_CODE_NONE)
	{
		auto application = m_embedClient.GetApplication();
		application->NotifyKeyUp(translatedKeyCode);
	}
	return TRUE;
}

long CWin32EmbedControl::OnTimer(WPARAM)
{
	if(!m_running) return FALSE;

	KillTimer(m_hWnd, TIMER_ID);

	HRESULT result = S_OK;

	if(m_embedClientActive)
	{
		auto application = m_embedClient.GetApplication();
		result = application->Update(1.f / 60.f);
		assert(SUCCEEDED(result));
		if(FAILED(result))
		{
			m_embedClientActive = false;
			ErrorRaised(this);
		}
	}

	if(m_embedClientActive)
	{
		SAFEARRAY* events = nullptr;
		auto application = m_embedClient.GetApplication();
		result = application->GetEvents(&events);
		assert(SUCCEEDED(result));
		if(events != nullptr)
		{
			void* eventStrings = nullptr;
			result = SafeArrayAccessData(events, &eventStrings);
			assert(SUCCEEDED(result));
			
			assert(events->cDims == 1);
			assert(events->rgsabound[0].lLbound == 0);
			for(unsigned int i = 0; i < events->rgsabound[0].cElements; i++)
			{
				BSTR eventStringValue = reinterpret_cast<BSTR*>(eventStrings)[i];
				NotificationRaised(string_cast<std::string>(eventStringValue));
			}
			
			result = SafeArrayUnaccessData(events);
			assert(SUCCEEDED(result));

			result = SafeArrayDestroy(events);
			assert(SUCCEEDED(result));
		}
	}

	static const float clearColor[4] = { 0, 0, 0, 0 };
	m_deviceContext->ClearRenderTargetView(m_outputTextureView, clearColor);

	if(m_embedClientActive)
	{
		result = m_sharedTextureMutex->AcquireSync(1, INFINITE);
		if(result != WAIT_OBJECT_0) return FALSE;

		m_deviceContext->CopyResource(m_outputTexture, m_sharedTexture);

		result = m_sharedTextureMutex->ReleaseSync(0);
		assert(SUCCEEDED(result));
	}

	m_swapChain->Present(0, 0);

	SetTimer(m_hWnd, TIMER_ID, TIMER_DELAY, 0);

	return FALSE;
}

long CWin32EmbedControl::OnSize(unsigned int, unsigned int width, unsigned int height)
{
	HRESULT result = S_OK;
	
	if(!m_swapChain.IsEmpty())
	{
		auto surfaceSize = GetSurfaceSize();

		m_outputTexture.Reset();
		m_outputTextureView.Reset();
		m_sharedTexture.Reset();
		m_sharedTextureMutex.Reset();

		result = m_swapChain->ResizeBuffers(1, surfaceSize.Width(), surfaceSize.Height(), DXGI_FORMAT_R8G8B8A8_UNORM, 0);
		assert(SUCCEEDED(result));

		CreateOutputTexture();
		CreateSharedTexture();
	}

	return FALSE;
}

long CWin32EmbedControl::OnGetDlgCode(WPARAM, LPARAM)
{
	return DLGC_WANTALLKEYS;
}
