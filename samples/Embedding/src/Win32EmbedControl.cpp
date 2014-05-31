#include "Win32EmbedControl.h"

using namespace Palleon;

#define CLSNAME							_T("Win32EmbedControl")
#define WNDSTYLE						(WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_TABSTOP)
#define WNDSTYLEEX						(0)

#define TIMER_DELAY						16
#define TIMER_ID						1

CWin32EmbedControl::CWin32EmbedControl(HWND parentWnd, const RECT& windowRect)
: m_embedClient(_T("ShadowMapping.exe"), _T("C:\\Projects\\Palleon\\samples\\ShadowMapping"))
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

	SetTimer(m_hWnd, TIMER_ID, TIMER_DELAY, 0);
}

CWin32EmbedControl::~CWin32EmbedControl()
{

}

void CWin32EmbedControl::CreateDevice()
{
	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};

	HRESULT result = S_OK;

	auto clientRect = GetClientRect();

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
	HRESULT result = S_OK;
	auto surfaceSize = GetSurfaceSize();

	auto application = m_embedClient.GetApplication();

	result = application->SetSurfaceSize(surfaceSize.Width(), surfaceSize.Height());
	assert(SUCCEEDED(result));

	HANDLE surfaceHandle = INVALID_HANDLE_VALUE;
	result = application->GetSurfaceHandle(reinterpret_cast<DWORD_PTR*>(&surfaceHandle));
	assert(SUCCEEDED(result));

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

long CWin32EmbedControl::OnMouseMove(WPARAM, int posX, int posY)
{
	auto application = m_embedClient.GetApplication();
	HRESULT result = application->NotifyMouseMove(posX, posY);
	assert(SUCCEEDED(result));
	return FALSE;
}

long CWin32EmbedControl::OnLeftButtonDown(int, int)
{
	auto application = m_embedClient.GetApplication();
	HRESULT result = application->NotifyMouseDown();
	assert(SUCCEEDED(result));
	return FALSE;
}

long CWin32EmbedControl::OnLeftButtonUp(int, int)
{
	auto application = m_embedClient.GetApplication();
	HRESULT result = application->NotifyMouseUp();
	assert(SUCCEEDED(result));
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

long CWin32EmbedControl::OnTimer(WPARAM)
{
	KillTimer(m_hWnd, TIMER_ID);

#if 0
	static LARGE_INTEGER currCall = { 0, 0 }, prevCall = { 0, 0 };

	QueryPerformanceCounter(&currCall);
	if(prevCall.QuadPart != 0)
	{
		LARGE_INTEGER freq = { 0, 0 };
		QueryPerformanceFrequency(&freq);
		double delta = static_cast<double>(currCall.QuadPart - prevCall.QuadPart) / static_cast<double>(freq.QuadPart);
		auto deltaString = std::to_string(delta);
		OutputDebugStringA((std::string("Delta: ") + deltaString + "\r\n").c_str());
	}
	prevCall = currCall;
#endif

	HRESULT result = S_OK;
	
	auto application = m_embedClient.GetApplication();
	result = application->Update(1.f / 60.f);
	assert(SUCCEEDED(result));

	static const float clearColor[4] = { 1, 0, 0, 0 };
	m_deviceContext->ClearRenderTargetView(m_outputTextureView, clearColor);

	result = m_sharedTextureMutex->AcquireSync(1, INFINITE);
	if(result != WAIT_OBJECT_0) return FALSE;

	m_deviceContext->CopyResource(m_outputTexture, m_sharedTexture);

	result = m_sharedTextureMutex->ReleaseSync(0);
	assert(SUCCEEDED(result));

	m_swapChain->Present(0, 0);

	SetTimer(m_hWnd, TIMER_ID, TIMER_DELAY, 0);

	return FALSE;
}
