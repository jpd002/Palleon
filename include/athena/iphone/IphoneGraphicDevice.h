#ifndef _IPHONEGRAPHICDEVICE_H_
#define _IPHONEGRAPHICDEVICE_H_

#include <vector>
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#include "athena/GraphicDevice.h"
#include "athena/Mesh.h"
#include "athena/IphoneEffectGenerator.h"

namespace Athena
{
	class CIphoneGraphicDevice : public CGraphicDevice
	{
	public:
		static void							CreateInstance(bool, const CVector2&);
		static void							DestroyInstance();

		virtual void						Draw();
		
		virtual VertexBufferPtr				CreateVertexBuffer(const VERTEX_BUFFER_DESCRIPTOR&);
		
		virtual TexturePtr					CreateTextureFromFile(const char*);
		virtual TexturePtr					CreateTextureFromMemory(const void*, uint32);
		virtual TexturePtr					CreateTextureFromRawData(const void*, TEXTURE_FORMAT, uint32, uint32);
		
		virtual TexturePtr					CreateCubeTextureFromFile(const char*);
		
		virtual RenderTargetPtr				CreateRenderTarget(TEXTURE_FORMAT, uint32, uint32);
		virtual CubeRenderTargetPtr			CreateCubeRenderTarget(TEXTURE_FORMAT, uint32);
		
		void								SetFrameRate(float);
		
	protected:
		enum
		{
			MAX_DIFFUSE_SLOTS = 5
		};
		
		struct EFFECTINFO
		{
			GLuint		program;
			
			GLuint		viewProjMatrixHandle;
			GLuint		worldMatrixHandle;
			GLuint		meshColorHandle;
			
			GLuint		diffuseTexture[MAX_DIFFUSE_SLOTS];
		};
		
		typedef std::vector<CMesh*> RenderQueue;
		typedef std::unordered_map<uint32, EFFECTINFO> EffectMap;

											CIphoneGraphicDevice(bool, const CVector2&);
		virtual								~CIphoneGraphicDevice();
		
		void								DrawViewport(CViewport*);

		void								GenerateEffect(const CIphoneEffectGenerator::EFFECTCAPS&);

		static GLuint						CompileShader(const char*, GLuint);
		void								DumpProgramLog(GLuint);
		GLuint								BuildProgram(const CIphoneEffectGenerator::EFFECTCAPS&);
		
		bool								FillRenderQueue(CSceneNode*, CCamera*);
		void								DrawMesh(CMesh*);

		EffectMap							m_effects;
		RenderQueue							m_renderQueue;
		bool								m_hasRetinaDisplay;
		
		CMatrix4							m_viewProjMatrix;
	};
}

#endif
