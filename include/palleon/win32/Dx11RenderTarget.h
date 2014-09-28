#pragma once

#include "../RenderTarget.h"

namespace Palleon
{
	class CDx11RenderTarget : public CRenderTarget
	{
	public:
								CDx11RenderTarget(ID3D11Device*, ID3D11DeviceContext*, TEXTURE_FORMAT, uint32, uint32);
		virtual					~CDx11RenderTarget();

		void					Update(uint32, const void*) override;
		void					UpdateCubeFace(TEXTURE_CUBE_FACE, const void*) override;
		void*					GetHandle() const override;

		void					Draw(const ViewportPtr&) override;

	private:
		typedef Framework::Win32::CComPtr<ID3D11Texture2D> TexturePtr;
		typedef Framework::Win32::CComPtr<ID3D11ShaderResourceView> ShaderResourceViewPtr;
		typedef Framework::Win32::CComPtr<ID3D11RenderTargetView> RenderTargetViewPtr;

		TexturePtr				m_texture;
		ShaderResourceViewPtr	m_shaderResourceView;
		RenderTargetViewPtr		m_renderTargetView;
		uint32					m_width = 0;
		uint32					m_height = 0;
	};
}