#include "palleon/win32/Win32EmbedServer.h"
#include "palleon/win32/Win32ResourceManager.h"
#include "palleon/win32/Dx11GraphicDevice.h"
#include "MemStream.h"
#include "win32/ComPtr.h"
#include "win32/ComStreamAdapter.h"
#include "string_cast.h"

using namespace Palleon;

CWin32EmbedServer::CWin32EmbedServer()
{
	CWin32ResourceManager::CreateInstance();
	CDx11GraphicDevice::CreateInstance(NULL, CVector2(640, 480), CVector2(640, 480));

	m_application = CreateApplication(true);
}

CWin32EmbedServer::~CWin32EmbedServer()
{

}

void CWin32EmbedServer::Loop()
{
	WriteInterfaceToConsole();

	MSG msg;
	while(GetMessage(&msg, 0, 0, 0) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

void CWin32EmbedServer::WriteInterfaceToConsole()
{
	Framework::CMemStream marshalStream;

	{
		auto comStream = Framework::Win32::CComPtr<Framework::Win32::CComStreamAdapter>(
			new Framework::Win32::CComStreamAdapter(marshalStream));

		DWORD dstContext = 0;
		HRESULT result = S_OK;
		result = CoMarshalInterface(comStream, IID_IEmbedApplication, this, MSHCTX_LOCAL, NULL, 0);
		assert(SUCCEEDED(result));
	}

	{
		BOOL result = FALSE;
		DWORD numberWritten = 0;

		HANDLE stdoutHandle = GetStdHandle(STD_OUTPUT_HANDLE);
		assert(stdoutHandle != 0);

		DWORD marshalSize = marshalStream.GetLength();
		result = WriteFile(stdoutHandle, &marshalSize, sizeof(DWORD), &numberWritten, nullptr);
		assert(result);
		assert(numberWritten == sizeof(DWORD));

		result = WriteFile(stdoutHandle, marshalStream.GetBuffer(), marshalSize, &numberWritten, nullptr);
		assert(result);
		assert(numberWritten == marshalSize);
	}
}

STDMETHODIMP_(ULONG) CWin32EmbedServer::AddRef()
{
	return 1;
}

STDMETHODIMP_(ULONG) CWin32EmbedServer::Release()
{
	return 1;
}

STDMETHODIMP CWin32EmbedServer::QueryInterface(const IID& iid, void** intrf)
{
	(*intrf) = NULL;
	if(iid == IID_IUnknown)
	{
		(*intrf) = static_cast<IUnknown*>(this);
	}
	else if(iid == IID_IEmbedApplication)
	{
		(*intrf) = static_cast<IEmbedApplication*>(this);
	}

	if(*intrf)
	{
		reinterpret_cast<IUnknown*>(*intrf)->AddRef();
		return S_OK;
	}
	else
	{
		return E_NOINTERFACE;
	}
}

STDMETHODIMP CWin32EmbedServer::Update(float dt)
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

	m_application->Update(dt);
	CGraphicDevice::GetInstance().Draw();
	return S_OK;
}

STDMETHODIMP CWin32EmbedServer::GetSurfaceHandle(DWORD_PTR* surfaceHandleReturn)
{
	HANDLE surfaceHandle = static_cast<CDx11GraphicDevice&>(CGraphicDevice::GetInstance()).GetOutputBufferSharedHandle();
	if(surfaceHandleReturn)
	{
		(*surfaceHandleReturn) = reinterpret_cast<DWORD_PTR>(surfaceHandle);
	}
	return S_OK;
}

STDMETHODIMP CWin32EmbedServer::SetSurfaceSize(unsigned int width, unsigned int height)
{
	CVector2 screenSize(width, height);
	static_cast<CDx11GraphicDevice&>(CGraphicDevice::GetInstance()).SetOutputBufferSize(screenSize, screenSize);
	m_application->NotifySizeChanged();
	return S_OK;
}

STDMETHODIMP CWin32EmbedServer::NotifyMouseMove(int mouseX, int mouseY)
{
	m_mouseX = mouseX;
	m_mouseY = mouseY;
	return S_OK;
}

STDMETHODIMP CWin32EmbedServer::NotifyMouseWheel(int mouseZ)
{
	m_mouseZ += mouseZ;
	return S_OK;
}

STDMETHODIMP CWin32EmbedServer::NotifyMouseDown()
{
	m_mouseDownPending = true;
	return S_OK;
}

STDMETHODIMP CWin32EmbedServer::NotifyMouseUp()
{
	m_mouseUpPending = true;
	return S_OK;
}

STDMETHODIMP CWin32EmbedServer::NotifyKeyDown(unsigned int code)
{
	m_application->NotifyKeyDown(static_cast<Palleon::KEY_CODE>(code));
	return S_OK;
}

STDMETHODIMP CWin32EmbedServer::NotifyKeyUp(unsigned int code)
{
	m_application->NotifyKeyUp(static_cast<Palleon::KEY_CODE>(code));
	return S_OK;
}

STDMETHODIMP CWin32EmbedServer::NotifyExternalCommand(BSTR command, BSTR* result)
{
	auto convertedCommand = string_cast<std::string>(command);
	auto commandResult = m_application->NotifyExternalCommand(convertedCommand);
	if(result)
	{
		(*result) = SysAllocString(string_cast<std::wstring>(commandResult.c_str()).c_str());
	}
	return S_OK;
}
