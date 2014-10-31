#pragma once

#include <d3d11.h>
#include <unordered_map>
#include "palleon/graphics/GraphicDevice.h"
#include "palleon/graphics/Mesh.h"
#include "palleon/win32/Dx11Effect.h"
#include "win32/ComPtr.h"

namespace Palleon
{
	class CDx11GraphicDevice : public CGraphicDevice
	{
	public:
		static void						CreateInstance(HWND, const CVector2&, const CVector2&);
		static void						DestroyInstance();

		virtual void					Draw() override;
		void							DrawViewportMainMap(CViewport*, ID3D11RenderTargetView*, ID3D11DepthStencilView*, uint32, uint32);

		virtual VertexBufferPtr			CreateVertexBuffer(const VERTEX_BUFFER_DESCRIPTOR&) override;

		virtual TexturePtr				CreateTexture(TEXTURE_FORMAT, uint32, uint32, uint32) override;
		virtual TexturePtr				CreateCubeTexture(TEXTURE_FORMAT, uint32) override;

		virtual RenderTargetPtr			CreateRenderTarget(TEXTURE_FORMAT, uint32, uint32) override;
		virtual CubeRenderTargetPtr		CreateCubeRenderTarget(TEXTURE_FORMAT, uint32) override;

		ID3D11Device*					GetDevice() const;
		ID3D11DeviceContext*			GetDeviceContext() const;
		HWND							GetParentWindow() const;

		HANDLE							GetOutputBufferSharedHandle();
		void							SetOutputBufferSize(const CVector2&, const CVector2&);

		void							SetFrameRate(float);

		static const DXGI_FORMAT		g_textureFormats[TEXTURE_FORMAT_MAX];

	protected:
		enum
		{
			MAX_PIXEL_SHADER_RESOURCE_SLOTS = CMaterial::MAX_TEXTURE_SLOTS + 1
		};

		struct RASTERIZER_STATE_INFO
		{
			unsigned int		cullMode				: 2;
		};
		static_assert(sizeof(RASTERIZER_STATE_INFO) == 4, "RASTERIZER_STATE_INFO's size must be 4 bytes.");

		typedef Framework::Win32::CComPtr<ID3D11RasterizerState> D3D11RasterizerStatePtr;
		typedef std::unordered_map<uint32, D3D11RasterizerStatePtr> RasterizerStateMap;

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

										CDx11GraphicDevice(HWND, const CVector2&, const CVector2&);
		virtual							~CDx11GraphicDevice();

		void							CreateDevice();
		void							CreateWindowlessDevice();
		void							CreateOutputBuffer();
		void							CreateWindowlessOutputBuffer();
		void							CreateDepthBuffer();
		void							CreateGlobalResources();
		void							CreateShadowMap();

		ID3D11BlendState*				GetBlendState(ALPHA_BLENDING_MODE);
		ID3D11RasterizerState*			GetRasterizerState(const RASTERIZER_STATE_INFO&);
		ID3D11DepthStencilState*		GetDepthStencilState(const DEPTHSTENCIL_STATE_INFO&);
		ID3D11SamplerState*				GetSamplerState(const SAMPLER_STATE_INFO&);

		void							DrawViewport(CViewport*);
		void							DrawViewportShadowMap(CViewport*);
		void							DrawMesh(const VIEWPORT_PARAMS&, CMesh*, const Dx11EffectPtr&);

		HWND													m_parentWnd;
		Framework::Win32::CComPtr<ID3D11Device>					m_device;
		Framework::Win32::CComPtr<ID3D11DeviceContext>			m_deviceContext;
		Framework::Win32::CComPtr<IDXGISwapChain>				m_swapChain;
		Framework::Win32::CComPtr<ID3D11Texture2D>				m_outputBuffer;
		Framework::Win32::CComPtr<IDXGIKeyedMutex>				m_outputBufferMutex;
		Framework::Win32::CComPtr<ID3D11RenderTargetView>		m_outputBufferView;
		Framework::Win32::CComPtr<ID3D11Texture2D>				m_depthBuffer;
		Framework::Win32::CComPtr<ID3D11DepthStencilView>		m_depthBufferView;
		Framework::Win32::CComPtr<ID3D11SamplerState>			m_defaultSamplerState;
		Framework::Win32::CComPtr<ID3D11BlendState>				m_blendStates[ALPHA_BLENDING_MODE_MAX];
		RasterizerStateMap										m_rasterizerStates;
		DepthStencilStateMap									m_depthStencilStates;
		SamplerStateMap											m_samplerStates;

		Framework::Win32::CComPtr<ID3D11Texture2D>				m_shadowMap;
		Framework::Win32::CComPtr<ID3D11ShaderResourceView>		m_shadowMapView;
		Framework::Win32::CComPtr<ID3D11Texture2D>				m_shadowDepthMap;
		Framework::Win32::CComPtr<ID3D11RenderTargetView>		m_shadowMapRenderView;
		Framework::Win32::CComPtr<ID3D11DepthStencilView>		m_shadowDepthMapView;

		RenderQueue						m_renderQueue;

		Dx11EffectPtr					m_shadowMapEffect;
	};
}
