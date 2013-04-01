#pragma once

#include <d3d11.h>
#include <unordered_map>
#include "../GraphicDevice.h"
#include "../Mesh.h"
#include "../win32/Dx11EffectGenerator.h"

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

//		ID3D11Device*					GetDevice() const;
		HWND							GetParentWindow() const;
//		void							DrawViewportToSurface(IDirect3DSurface9*, CViewport*);

		void							SetFrameRate(float);

//		static D3DFORMAT				ConvertTextureFormatId(TEXTURE_FORMAT);

	protected:
		enum
		{
			MAX_DIFFUSE_SLOTS = 5,
		};

		typedef std::unordered_map<uint64, ID3D11InputLayout*> InputLayoutMap;

		struct EFFECTINFO
		{
			ID3D11VertexShader*		vertexShader;
			ID3DBlob*				vertexShaderCode;
			ID3D11PixelShader*		pixelShader;
			ID3D11Buffer*			constantBuffer;
			InputLayoutMap			inputLayouts;

			uint32					worldMatrixOffset;
			uint32					viewProjMatrixOffset;
		};

		typedef std::vector<CMesh*> RenderQueue;
		typedef std::unordered_map<uint32, EFFECTINFO> EffectMap;

										CDx11GraphicDevice(HWND, const CVector2&);
		virtual							~CDx11GraphicDevice();

		void							CreateDevice();
		ID3D11InputLayout*				CreateInputLayout(const VERTEX_BUFFER_DESCRIPTOR&, ID3DBlob*);

		void							GenerateEffect(const CDx11EffectGenerator::EFFECTCAPS&);

		void							DrawViewport(CViewport*);
		bool							FillRenderQueue(CSceneNode*, CCamera*);
		void							DrawMesh(CMesh*);

		HWND							m_parentWnd;
		ID3D11Device*					m_device;
		ID3D11DeviceContext*			m_deviceContext;
		ID3D11RenderTargetView*			m_renderTargetView;
		ID3D11Texture2D*				m_depthBuffer;
		ID3D11DepthStencilState*		m_depthStencilState;
		ID3D11RasterizerState*			m_rasterState;
		IDXGISwapChain*					m_swapChain;
		EffectMap						m_effects;
		RenderQueue						m_renderQueue;

		CMatrix4						m_viewProjMatrix;
	};
}
