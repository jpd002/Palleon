#pragma once

#include "palleon/GraphicDevice.h"
#include "palleon/ios/MetalView.h"
#include "palleon/ios/MetalEffect.h"

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
											CMetalGraphicDevice(MetalView*);
		virtual								~CMetalGraphicDevice();
		
		void								DrawViewport(id<MTLRenderCommandEncoder>, CViewport*, unsigned int&);
		void								DrawViewportMainMap(id<MTLRenderCommandEncoder>, CViewport*, unsigned int&);
		
		void								DrawMesh(id<MTLRenderCommandEncoder>, unsigned int, const METALVIEWPORT_PARAMS&, CMesh* mesh, const MetalEffectPtr& effect);
		
		MetalView*							m_metalView;
		id<MTLCommandQueue>					m_commandQueue;
		id<MTLBuffer>						m_constantBuffer;
		dispatch_semaphore_t				m_drawSemaphore;
	};
}
