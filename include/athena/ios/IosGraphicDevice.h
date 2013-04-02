#pragma once

#include <vector>
#include "OpenGlEsDefs.h"
#include "athena/GraphicDevice.h"
#include "athena/Mesh.h"
#include "athena/Ios/IosEffectGenerator.h"

namespace Athena
{
	class CIosGraphicDevice : public CGraphicDevice
	{
	public:
		enum VERTEX_ATTRIB
		{
			VERTEX_ATTRIB_POSITION	= 0,
			VERTEX_ATTRIB_TEXCOORD0	= 1,
			VERTEX_ATTRIB_TEXCOORD1 = 2,
			VERTEX_ATTRIB_COLOR		= 3				
		};
		
		static void							CreateInstance(bool, const CVector2&);
		static void							DestroyInstance();

		virtual void						Draw();
		
		virtual VertexBufferPtr				CreateVertexBuffer(const VERTEX_BUFFER_DESCRIPTOR&);
		
		virtual TexturePtr					CreateTexture(TEXTURE_FORMAT, uint32, uint32);
		virtual TexturePtr					CreateTextureFromFile(const char*);
		virtual TexturePtr					CreateTextureFromMemory(const void*, uint32);
		virtual TexturePtr					CreateTextureFromRawData(const void*, TEXTURE_FORMAT, uint32, uint32);
		
		virtual void						UpdateTexture(const TexturePtr&, const void*);
		
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
			GLuint		diffuseTextureMatrix[MAX_DIFFUSE_SLOTS];
		};
		
		typedef std::vector<CMesh*> RenderQueue;
		typedef std::unordered_map<uint32, EFFECTINFO> EffectMap;

											CIosGraphicDevice(bool, const CVector2&);
		virtual								~CIosGraphicDevice();
		
		void								DrawViewport(CViewport*);

		void								GenerateEffect(const CIosEffectGenerator::EFFECTCAPS&);

		static GLuint						CompileShader(const char*, GLuint);
		void								DumpProgramLog(GLuint);
		GLuint								BuildProgram(const CIosEffectGenerator::EFFECTCAPS&);
		
		bool								FillRenderQueue(CSceneNode*, CCamera*);
		void								DrawMesh(CMesh*);

		EffectMap							m_effects;
		RenderQueue							m_renderQueue;
		bool								m_hasRetinaDisplay;
		
		CMatrix4							m_viewProjMatrix;
		CMatrix4							m_peggedViewProjMatrix;
	};
}