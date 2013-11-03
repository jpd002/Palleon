#pragma once

#include <d3d11.h>
#include <unordered_map>
#include "../GraphicDevice.h"
#include "../Mesh.h"
#include "../win32/Dx11EffectGenerator.h"
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
//		void							DrawViewportToSurface(IDirect3DSurface9*, CViewport*);

		void							SetFrameRate(float);

//		static D3DFORMAT				ConvertTextureFormatId(TEXTURE_FORMAT);

	protected:
		enum
		{
			MAX_DIFFUSE_SLOTS = 5,
		};

		enum
		{
			MAX_PIXEL_SHADER_RESOURCE_SLOTS = MAX_DIFFUSE_SLOTS + 1
		};

		typedef Framework::Win32::CComPtr<ID3D11InputLayout> D3D11InputLayoutPtr;
		typedef std::unordered_map<uint64, D3D11InputLayoutPtr> InputLayoutMap;

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

		struct EFFECTINFO
		{
			EFFECTINFO()
			: meshColorOffset(-1)
			, worldMatrixOffset(-1)
			, viewProjMatrixOffset(-1)
			, shadowViewProjMatrixOffset(-1)
			{
				for(unsigned int i = 0; i < MAX_DIFFUSE_SLOTS; i++)
				{
					diffuseTextureMatrixOffset[i] = -1;
				}
			}

			Framework::Win32::CComPtr<ID3D11VertexShader>	vertexShader;
			Framework::Win32::CComPtr<ID3DBlob>				vertexShaderCode;
			Framework::Win32::CComPtr<ID3D11PixelShader>	pixelShader;
			Framework::Win32::CComPtr<ID3D11Buffer>			constantBuffer;

			InputLayoutMap			inputLayouts;

			uint32					meshColorOffset;
			uint32					worldMatrixOffset;
			uint32					viewProjMatrixOffset;
			uint32					shadowViewProjMatrixOffset;
			uint32					diffuseTextureMatrixOffset[MAX_DIFFUSE_SLOTS];
		};

		typedef std::vector<CMesh*> RenderQueue;
		typedef std::unordered_map<uint32, EFFECTINFO> EffectMap;

										CDx11GraphicDevice(HWND, const CVector2&);
		virtual							~CDx11GraphicDevice();

		void							CreateDevice();
		void							CreateShadowMap();
		D3D11InputLayoutPtr				CreateInputLayout(const VERTEX_BUFFER_DESCRIPTOR&, ID3DBlob*);

		void							GenerateEffect(const CDx11EffectGenerator::EFFECTCAPS&);
		EFFECTINFO						GenerateShadowMapEffect();

		ID3D11BlendState*				GetBlendState(ALPHA_BLENDING_MODE);
		ID3D11DepthStencilState*		GetDepthStencilState(const DEPTHSTENCIL_STATE_INFO&);

		void							DrawViewport(CViewport*);
		void							DrawViewportMainMap(CViewport*);
		void							DrawViewportShadowMap(CViewport*);
		void							DrawMesh(CMesh*, EFFECTINFO*, const CMatrix4&, const CMatrix4& = CMatrix4::MakeIdentity());

		EFFECTINFO*						GetEffectFromMesh(CMesh*, bool);

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

		Framework::Win32::CComPtr<ID3D11Texture2D>				m_shadowMap;
		Framework::Win32::CComPtr<ID3D11ShaderResourceView>		m_shadowMapView;
		Framework::Win32::CComPtr<ID3D11Texture2D>				m_shadowDepthMap;
		Framework::Win32::CComPtr<ID3D11RenderTargetView>		m_shadowMapRenderView;
		Framework::Win32::CComPtr<ID3D11DepthStencilView>		m_shadowDepthMapView;

		EffectMap						m_effects;
		RenderQueue						m_renderQueue;

		EFFECTINFO						m_shadowMapEffect;
	};
}
