#pragma once

#include <d3d11.h>
#include "win32/Window.h"
#include "win32/ComPtr.h"
#include "palleon/win32/Win32EmbedClient.h"

namespace Palleon
{
	class CWin32EmbedControl : public Framework::Win32::CWindow
	{
	public:
										CWin32EmbedControl(HWND, const RECT&);
		virtual							~CWin32EmbedControl();

	protected:
		long							OnMouseMove(WPARAM, int, int) override;
		long							OnLeftButtonDown(int, int) override;
		long							OnLeftButtonUp(int, int) override;
		long							OnTimer(WPARAM) override;
		long							OnSize(unsigned int, unsigned int, unsigned int) override;

	private:
		typedef Framework::Win32::CComPtr<ID3D11Device> D3D11DevicePtr;
		typedef Framework::Win32::CComPtr<ID3D11DeviceContext> D3D11DeviceContextPtr;
		typedef Framework::Win32::CComPtr<ID3D11RenderTargetView> D3D11RenderTargetViewPtr;
		typedef Framework::Win32::CComPtr<ID3D11Texture2D> D3D11Texture2DPtr;
		typedef Framework::Win32::CComPtr<IDXGISwapChain> DXGISwapChainPtr;
		typedef Framework::Win32::CComPtr<IDXGIKeyedMutex> DXGIKeyedMutexPtr;

		void							CreateDevice();
		void							CreateSharedTexture();

		D3D11DevicePtr					m_device;
		D3D11DeviceContextPtr			m_deviceContext;
		D3D11RenderTargetViewPtr		m_renderTargetView;
		D3D11Texture2DPtr				m_backBufferTexture;
		D3D11Texture2DPtr				m_sharedTexture;
		DXGIKeyedMutexPtr				m_sharedTextureMutex;
		DXGISwapChainPtr				m_swapChain;

		CWin32EmbedClient				m_embedClient;
	};
}
