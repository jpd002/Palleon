#pragma once

#include <d3d11.h>
#include <boost/signals2.hpp>
#include "win32/Window.h"
#include "win32/ComPtr.h"
#include "palleon/win32/Win32EmbedClient.h"
#include "palleon/Application.h"

namespace Palleon
{
	class CWin32EmbedControl : public Framework::Win32::CWindow
	{
	public:
		typedef boost::signals2::signal<void (CWin32EmbedControl*)> ErrorRaisedEventType;

										CWin32EmbedControl(HWND, const RECT&, const std::tstring&, const std::tstring&);
		virtual							~CWin32EmbedControl();

		bool							IsClientActive() const;

		std::string						ExecuteCommand(const std::string&);

		ErrorRaisedEventType			ErrorRaised;

	protected:
		long							OnMouseMove(WPARAM, int, int) override;
		long							OnMouseWheel(int, int, short) override;
		long							OnLeftButtonDown(int, int) override;
		long							OnLeftButtonUp(int, int) override;
		long							OnKeyDown(WPARAM, LPARAM) override;
		long							OnKeyUp(WPARAM, LPARAM) override;
		long							OnTimer(WPARAM) override;
		long							OnSize(unsigned int, unsigned int, unsigned int) override;
		long							OnGetDlgCode(WPARAM, LPARAM) override;

	private:
		typedef Framework::Win32::CComPtr<ID3D11Device> D3D11DevicePtr;
		typedef Framework::Win32::CComPtr<ID3D11DeviceContext> D3D11DeviceContextPtr;
		typedef Framework::Win32::CComPtr<ID3D11RenderTargetView> D3D11RenderTargetViewPtr;
		typedef Framework::Win32::CComPtr<ID3D11Texture2D> D3D11Texture2DPtr;
		typedef Framework::Win32::CComPtr<IDXGISwapChain> DXGISwapChainPtr;
		typedef Framework::Win32::CComPtr<IDXGIKeyedMutex> DXGIKeyedMutexPtr;

		void							CreateDevice();
		void							CreateOutputTexture();
		void							CreateSharedTexture();

		Framework::Win32::CRect			GetSurfaceSize();

		KEY_CODE						TranslateKeyCode(unsigned int);

		D3D11DevicePtr					m_device;
		D3D11DeviceContextPtr			m_deviceContext;
		D3D11Texture2DPtr				m_outputTexture;
		D3D11RenderTargetViewPtr		m_outputTextureView;
		D3D11Texture2DPtr				m_sharedTexture;
		DXGIKeyedMutexPtr				m_sharedTextureMutex;
		DXGISwapChainPtr				m_swapChain;

		CWin32EmbedClient				m_embedClient;
		bool							m_embedClientActive = true;
	};
}
