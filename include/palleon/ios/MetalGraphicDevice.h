#pragma once

#include "palleon/GraphicDevice.h"
#include "palleon/ios/MetalView.h"
#include "palleon/ios/MetalEffect.h"
#include "palleon/ios/MetalShadowMapEffectProvider.h"

namespace Palleon
{
	class CMetalGraphicDevice : public CGraphicDevice
	{
	public:
		static void							CreateInstance(MetalView*);
		static void							DestroyInstance();
		
		virtual void						Draw() override;
		
		virtual VertexBufferPtr				CreateVertexBuffer(const VERTEX_BUFFER_DESCRIPTOR&) override;
		
		virtual TexturePtr					CreateTexture(TEXTURE_FORMAT, uint32, uint32, uint32) override;
		virtual TexturePtr					CreateCubeTexture(TEXTURE_FORMAT, uint32) override;
		
		virtual RenderTargetPtr				CreateRenderTarget(TEXTURE_FORMAT, uint32, uint32) override;
		virtual CubeRenderTargetPtr			CreateCubeRenderTarget(TEXTURE_FORMAT, uint32) override;
		
	private:
		enum
		{
			CONSTANT_BUFFER_SIZE = 0x10000,
		};
		
		struct SAMPLER_STATE_INFO
		{
			unsigned int		addressU		: 1;
			unsigned int		addressV		: 1;
			unsigned int		reserved		: 30;
		};
		static_assert(sizeof(SAMPLER_STATE_INFO) == 4, "SAMPLER_STATE_INFO's size must be 4 bytes.");
		
		typedef std::unordered_map<uint32, id<MTLSamplerState>> SamplerStateMap;
		
											CMetalGraphicDevice(MetalView*);
		virtual								~CMetalGraphicDevice();
		
		void								CreateDepthStencilState();
		void								CreateShadowMap();
		void								UpdateDepthBuffer();
		
		void								DrawViewportMainMap(id<MTLRenderCommandEncoder>, CViewport*, unsigned int&);
		void								DrawViewportShadowMap(id<MTLRenderCommandEncoder>, CViewport*, unsigned int&);
		
		void								DrawMesh(id<MTLRenderCommandEncoder>, unsigned int, const METALVIEWPORT_PARAMS&, CMesh* mesh, const MetalEffectPtr& effect);
		
		void								SetupMainRenderPass(id<CAMetalDrawable>);
		id<MTLSamplerState>					GetSamplerState(const SAMPLER_STATE_INFO&);
		
		MetalView*							m_metalView = nil;
		id<MTLCommandQueue>					m_commandQueue = nil;
		id<MTLBuffer>						m_constantBuffer = nil;
		
		MTLRenderPassDescriptor*			m_mainRenderPass = nil;
		id<MTLTexture>						m_mainDepthBuffer = nil;

		id<MTLTexture>						m_shadowMap = nil;
		MTLRenderPassDescriptor*			m_shadowRenderPass = nil;
		
		SamplerStateMap						m_samplerStates;
		id<MTLDepthStencilState>			m_depthStencilState = nil;
		dispatch_semaphore_t				m_drawSemaphore;
		
		EffectProviderPtr					m_shadowMapEffectProvider;
	};
}
