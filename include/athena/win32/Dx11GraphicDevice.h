#pragma once

#include <d3d11.h>
#include <unordered_map>
#include "../GraphicDevice.h"
#include "../Mesh.h"
#include "../win32/Dx11Effect.h"
#include "win32/ComPtr.h"

namespace Athena
{
	class CDx11GraphicDevice : public CGraphicDevice
	{
	public:
		static void						CreateInstance(HWND, const CVector2&);
		static void						DestroyInstance();

		virtual void					Draw();

		virtual VertexBufferPtr			CreateVertexBuffer(const VERTEX_BUFFER_DESCRIPTOR&);

		virtual TexturePtr				CreateTexture(TEXTURE_FORMAT, uint32, uint32);
		virtual TexturePtr				CreateTextureFromFile(const char*);
		virtual TexturePtr				CreateTextureFromMemory(const void*, uint32);
		virtual TexturePtr				CreateTextureFromRawData(const void*, TEXTURE_FORMAT, uint32, uint32);

		virtual void					UpdateTexture(const TexturePtr&, const void*);

		virtual TexturePtr				CreateCubeTextureFromFile(const char*);

		virtual RenderTargetPtr			CreateRenderTarget(TEXTURE_FORMAT, uint32, uint32);
		virtual CubeRenderTargetPtr		CreateCubeRenderTarget(TEXTURE_FORMAT, uint32);

		HWND							GetParentWindow() const;

		void							SetFrameRate(float);

	protected:
		enum
		{
			MAX_DIFFUSE_SLOTS = 5,
		};

		enum
		{
			MAX_PIXEL_SHADER_RESOURCE_SLOTS = MAX_DIFFUSE_SLOTS + 1
		};

		struct DEPTHSTENCIL_STATE_INFO
		{
			unsigned int		depthWriteEnabled		: 1;
			unsigned int		stencilTestEnabled		: 1;
			unsigned int		stencilFunction			: 4;
			unsigned int		stencilFailAction		: 4;
			unsigned int		reserved				: 22;
		};
		static_assert(sizeof(DEPTHSTENCIL_STATE_INFO) == 4, "DEPTHSTENCIL_STATE_INFO's size must be 4 bytes.");

		typedef Framework::Win32::CComPtr<ID3D11DepthStencilState> D3D11DepthStencilStatePtr;
		typedef std::unordered_map<uint32, D3D11DepthStencilStatePtr> DepthStencilStateMap;

		struct SAMPLER_STATE_INFO
		{
			unsigned int		addressU		: 1;
			unsigned int		addressV		: 1;
			unsigned int		reserved		: 30;
		};
		static_assert(sizeof(SAMPLER_STATE_INFO) == 4, "SAMPLER_STATE_INFO's size must be 4 bytes.");

		typedef Framework::Win32::CComPtr<ID3D11SamplerState> D3D11SamplerStatePtr;
		typedef std::unordered_map<uint32, D3D11SamplerStatePtr> SamplerStateMap;

		typedef std::vector<CMesh*> RenderQueue;
		typedef std::unordered_map<uint32, Dx11EffectPtr> EffectMap;

										CDx11GraphicDevice(HWND, const CVector2&);
		virtual							~CDx11GraphicDevice();

		void							CreateDevice();
		void							CreateShadowMap();

		ID3D11BlendState*				GetBlendState(ALPHA_BLENDING_MODE);
		ID3D11DepthStencilState*		GetDepthStencilState(const DEPTHSTENCIL_STATE_INFO&);
		ID3D11SamplerState*				GetSamplerState(const SAMPLER_STATE_INFO&);

		void							DrawViewport(CViewport*);
		void							DrawViewportMainMap(CViewport*);
		void							DrawViewportShadowMap(CViewport*);
		void							DrawMesh(CMesh*, const Dx11EffectPtr&, const CMatrix4&, bool = false, const CMatrix4& = CMatrix4::MakeIdentity());

		Dx11EffectPtr					GetEffectFromMesh(CMesh*, bool);

		HWND													m_parentWnd;
		Framework::Win32::CComPtr<ID3D11Device>					m_device;
		Framework::Win32::CComPtr<ID3D11DeviceContext>			m_deviceContext;
		Framework::Win32::CComPtr<IDXGISwapChain>				m_swapChain;
		Framework::Win32::CComPtr<ID3D11RenderTargetView>		m_renderTargetView;
		Framework::Win32::CComPtr<ID3D11Texture2D>				m_depthBuffer;
		Framework::Win32::CComPtr<ID3D11DepthStencilView>		m_depthBufferView;
		Framework::Win32::CComPtr<ID3D11RasterizerState>		m_rasterState;
		Framework::Win32::CComPtr<ID3D11SamplerState>			m_defaultSamplerState;
		Framework::Win32::CComPtr<ID3D11BlendState>				m_blendStates[ALPHA_BLENDING_MODE_MAX];
		DepthStencilStateMap									m_depthStencilStates;
		SamplerStateMap											m_samplerStates;

		Framework::Win32::CComPtr<ID3D11Texture2D>				m_shadowMap;
		Framework::Win32::CComPtr<ID3D11ShaderResourceView>		m_shadowMapView;
		Framework::Win32::CComPtr<ID3D11Texture2D>				m_shadowDepthMap;
		Framework::Win32::CComPtr<ID3D11RenderTargetView>		m_shadowMapRenderView;
		Framework::Win32::CComPtr<ID3D11DepthStencilView>		m_shadowDepthMapView;

		EffectMap						m_effects;
		RenderQueue						m_renderQueue;

		Dx11EffectPtr					m_shadowMapEffect;
	};
}
