#pragma once

#include <Windows.h>
#include "win32/ComPtr.h"
#include "../../../src/win32/EmbedInterface_h.h"
#include "tcharx.h"

namespace Palleon
{
	class CWin32EmbedClient
	{
	public:
									CWin32EmbedClient(const std::tstring&, const std::tstring&);
		virtual						~CWin32EmbedClient();

		IEmbedApplication*			GetApplication();

	private:
		void						Launch(const std::tstring&, const std::tstring&);
		void						ConnectHandler();

		typedef Framework::Win32::CComPtr<IEmbedApplication> EmbedApplicationPtr;

		HANDLE						m_process = INVALID_HANDLE_VALUE;
		HANDLE						m_childStd_IN_Rd = INVALID_HANDLE_VALUE;
		HANDLE						m_childStd_IN_Wr = INVALID_HANDLE_VALUE;
		HANDLE						m_childStd_OUT_Rd = INVALID_HANDLE_VALUE;
		HANDLE						m_childStd_OUT_Wr = INVALID_HANDLE_VALUE;

		EmbedApplicationPtr			m_application;
	};
}
