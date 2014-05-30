#include <cassert>
#include <vector>
#include "palleon/win32/Win32EmbedClient.h"
#include "win32/ComStreamAdapter.h"
#include "PtrStream.h"
#include "string_format.h"

using namespace Palleon;

CWin32EmbedClient::CWin32EmbedClient(const std::tstring& executableName, const std::tstring& workingDir)
{
	Launch(executableName, workingDir);
	ConnectHandler();
}

CWin32EmbedClient::~CWin32EmbedClient()
{
	m_application.Reset();
	TerminateProcess(m_process, -1);
	CloseHandle(m_process);
	CloseHandle(m_childStd_IN_Rd);
	CloseHandle(m_childStd_IN_Wr);
	CloseHandle(m_childStd_OUT_Rd);
	CloseHandle(m_childStd_OUT_Wr);
}

IEmbedApplication* CWin32EmbedClient::GetApplication()
{
	return static_cast<IEmbedApplication*>(m_application);
}

void CWin32EmbedClient::Launch(const std::tstring& executableName, const std::tstring& workingDir)
{
	{
		SECURITY_ATTRIBUTES securityAttrib = {};
		securityAttrib.nLength = sizeof(SECURITY_ATTRIBUTES);
		securityAttrib.bInheritHandle = TRUE;
		securityAttrib.lpSecurityDescriptor = NULL;

		BOOL success = FALSE;

		success = CreatePipe(&m_childStd_OUT_Rd, &m_childStd_OUT_Wr, &securityAttrib, 0);
		assert(success);

		success = SetHandleInformation(m_childStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0);
		assert(success);

		success = CreatePipe(&m_childStd_IN_Rd, &m_childStd_IN_Wr, &securityAttrib, 0);
		assert(success);

		success = SetHandleInformation(m_childStd_IN_Wr, HANDLE_FLAG_INHERIT, 0);
		assert(success);
	}

	{
		STARTUPINFO startInfo = {};
		startInfo.cb = sizeof(STARTUPINFO); 
		startInfo.hStdError = m_childStd_OUT_Wr;
		startInfo.hStdOutput = m_childStd_OUT_Wr;
		startInfo.hStdInput = m_childStd_IN_Rd;
		startInfo.dwFlags |= STARTF_USESTDHANDLES;
 
		auto cmdLine = string_format(_T("%s embed"), executableName.c_str());

		PROCESS_INFORMATION procInfo = {};
		BOOL success = FALSE;
		success = CreateProcess(NULL, const_cast<TCHAR*>(cmdLine.data()), NULL, 
			NULL, TRUE, 0, NULL, workingDir.c_str(), &startInfo, &procInfo);
		assert(success);

		m_process = procInfo.hProcess;
		CloseHandle(procInfo.hThread);
	}
}

void CWin32EmbedClient::ConnectHandler()
{
	std::vector<uint8> marshalData;

	{
		BOOL result = FALSE;
		DWORD numberRead = 0;
		DWORD marshalSize = 0;
		
		result = ReadFile(m_childStd_OUT_Rd, &marshalSize, sizeof(DWORD), &numberRead, nullptr);
		assert(result);
		assert(numberRead == sizeof(DWORD));

		marshalData.resize(marshalSize);

		result = ReadFile(m_childStd_OUT_Rd, marshalData.data(), marshalSize, &numberRead, nullptr);
		assert(result);
		assert(numberRead == marshalSize);
	}

	Framework::CPtrStream marshalDataStream(marshalData.data(), marshalData.size());

	auto stream = Framework::Win32::CComPtr<Framework::Win32::CComStreamAdapter>(
		new Framework::Win32::CComStreamAdapter(marshalDataStream));
	
	HRESULT result = S_OK;
	result = CoUnmarshalInterface(stream, IID_IEmbedApplication, reinterpret_cast<LPVOID*>(&m_application));
	assert(SUCCEEDED(result));
}
