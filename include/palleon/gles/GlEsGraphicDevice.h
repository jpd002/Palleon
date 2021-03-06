#pragma once

#include <vector>
#include "OpenGlEsDefs.h"
#include "palleon/graphics/GraphicDevice.h"
#include "palleon/graphics/Mesh.h"
#include "palleon/gles/GlEsEffect.h"

namespace Palleon
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

		void								DrawViewportMainMap(CViewport*, uint32, uint32);
		
	protected:				
		typedef std::vector<CMesh*> RenderQueue;
				
											CGlEsGraphicDevice(const CVector2&, float);
		virtual								~CGlEsGraphicDevice();
		
		void								Initialize();
		
		void								DrawViewport(CViewport*);
		
		void								DrawViewportShadowMap(CViewport*);
		
		bool								FillRenderQueue(const SceneNodePtr&, CCamera*);
		void								DrawMesh(const VIEWPORT_PARAMS& viewportParams, CMesh* mesh, const GlEsEffectPtr& effect);

		void								CreateShadowMap();
		
		bool								m_hasVertexArrayObjects = false;

		GlEsEffectPtr						m_shadowMapEffect;
		
		RenderQueue							m_renderQueue;
				
		GLuint								m_mainFramebuffer = 0;
		
		GLuint								m_shadowMapTexture = 0;
		GLuint								m_shadowMapDepthbuffer = 0;
		GLuint								m_shadowMapFramebuffer = 0;
	};
}
