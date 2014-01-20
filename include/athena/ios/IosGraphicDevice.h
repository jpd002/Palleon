#pragma once

#include <vector>
#include "OpenGlEsDefs.h"
#include "athena/GraphicDevice.h"
#include "athena/Mesh.h"
#include "athena/Ios/IosUberEffectGenerator.h"
#include "athena/Ios/IosEffect.h"

namespace Athena
{
	class CIosGraphicDevice : public CGraphicDevice
	{
	public:
		static void							CreateInstance(bool, const CVector2&);
		static void							DestroyInstance();

		void								SetMainFramebuffer(GLuint);
		
		virtual void						Draw() override;
		
		virtual VertexBufferPtr				CreateVertexBuffer(const VERTEX_BUFFER_DESCRIPTOR&) override;
		
		virtual TexturePtr					CreateTexture(TEXTURE_FORMAT, uint32, uint32) override;
		virtual TexturePtr					CreateTextureFromFile(const char*) override;
		virtual TexturePtr					CreateTextureFromMemory(const void*, uint32) override;
		virtual TexturePtr					CreateTextureFromRawData(const void*, TEXTURE_FORMAT, uint32, uint32) override;
		
		virtual TexturePtr					CreateCubeTexture(TEXTURE_FORMAT, uint32) override;
		virtual TexturePtr					CreateCubeTextureFromFile(const char*) override;
		
		virtual RenderTargetPtr				CreateRenderTarget(TEXTURE_FORMAT, uint32, uint32) override;
		virtual CubeRenderTargetPtr			CreateCubeRenderTarget(TEXTURE_FORMAT, uint32) override;
		
		void								SetFrameRate(float);
		
	protected:				
		typedef std::vector<CMesh*> RenderQueue;

											CIosGraphicDevice(bool, const CVector2&);
		virtual								~CIosGraphicDevice();
				
		void								DrawViewport(CViewport*);
		
		void								DrawViewportMainMap(CViewport*);
		void								DrawViewportShadowMap(CViewport*);
		
		bool								FillRenderQueue(const SceneNodePtr&, CCamera*);
		void								DrawMesh(CMesh*, const IosEffectPtr&, const CMatrix4&, bool = false, const CMatrix4& = CMatrix4());

		void								CreateShadowMap();
		
		IosEffectPtr						m_shadowMapEffect;
		
		RenderQueue							m_renderQueue;
		bool								m_hasRetinaDisplay;
				
		GLuint								m_mainFramebuffer = 0;
		
		GLuint								m_shadowMapTexture = 0;
		GLuint								m_shadowMapDepthbuffer = 0;
		GLuint								m_shadowMapFramebuffer = 0;
	};
}
