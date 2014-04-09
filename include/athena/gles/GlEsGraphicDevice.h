#pragma once

#include <vector>
#include "OpenGlEsDefs.h"
#include "athena/GraphicDevice.h"
#include "athena/Mesh.h"
#include "athena/gles/GlEsEffect.h"

namespace Athena
{
	class CGlEsGraphicDevice : public CGraphicDevice
	{
	public:
		void								SetMainFramebuffer(GLuint);
		
		virtual void						Draw() override;
		
		virtual VertexBufferPtr				CreateVertexBuffer(const VERTEX_BUFFER_DESCRIPTOR&) override;
		
		virtual TexturePtr					CreateTexture(TEXTURE_FORMAT, uint32, uint32, uint32) override;
		virtual TexturePtr					CreateCubeTexture(TEXTURE_FORMAT, uint32) override;
		
		virtual RenderTargetPtr				CreateRenderTarget(TEXTURE_FORMAT, uint32, uint32) override;
		virtual CubeRenderTargetPtr			CreateCubeRenderTarget(TEXTURE_FORMAT, uint32) override;
		
		void								SetFrameRate(float);
		
	protected:				
		typedef std::vector<CMesh*> RenderQueue;
				
											CGlEsGraphicDevice(const CVector2&);
		virtual								~CGlEsGraphicDevice();
		
		void								Initialize();
		
		void								DrawViewport(CViewport*);
		
		void								DrawViewportMainMap(CViewport*);
		void								DrawViewportShadowMap(CViewport*);
		
		bool								FillRenderQueue(const SceneNodePtr&, CCamera*);
		void								DrawMesh(CMesh*, const GlEsEffectPtr&, const CMatrix4&, const CMatrix4&, bool = false, const CMatrix4& = CMatrix4());

		void								CreateShadowMap();
		
		GlEsEffectPtr						m_shadowMapEffect;
		
		RenderQueue							m_renderQueue;
				
		GLuint								m_mainFramebuffer = 0;
		
		GLuint								m_shadowMapTexture = 0;
		GLuint								m_shadowMapDepthbuffer = 0;
		GLuint								m_shadowMapFramebuffer = 0;
	};
}
