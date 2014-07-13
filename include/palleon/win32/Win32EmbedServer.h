#pragma once

#include "../Application.h"
#include "../../../src/win32/EmbedInterface_h.h"

namespace Palleon
{
	class CWin32EmbedServer : public IEmbedApplication
	{
	public:
								CWin32EmbedServer();
		virtual					~CWin32EmbedServer();

		void					Loop();

		//IUnknown
		STDMETHODIMP_(ULONG)	AddRef() override;
		STDMETHODIMP_(ULONG)	Release() override;
		STDMETHODIMP			QueryInterface(const IID&, void**) override;

		//IEmbedApplication
		STDMETHODIMP			Update(float) override;

		STDMETHODIMP			GetSurfaceHandle(DWORD_PTR*) override;
		STDMETHODIMP			SetSurfaceSize(unsigned int, unsigned int) override;

		STDMETHODIMP			NotifyMouseMove(int, int) override;
		STDMETHODIMP			NotifyMouseWheel(int) override;
		STDMETHODIMP			NotifyMouseDown() override;
		STDMETHODIMP			NotifyMouseUp() override;

		STDMETHODIMP			NotifyKeyDown(unsigned int) override;
		STDMETHODIMP			NotifyKeyUp(unsigned int) override;

		STDMETHODIMP			NotifyExternalCommand(BSTR, BSTR*) override;

	private:
		void					WriteInterfaceToConsole();

		CApplication*			m_application = nullptr;

		int						m_mouseX = 0;
		int						m_mouseY = 0;
		int						m_mouseZ = 0;
		bool					m_mouseDownPending = false;
		bool					m_mouseUpPending = false;
	};
}
