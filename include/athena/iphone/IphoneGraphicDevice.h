#ifndef _IPHONEGRAPHICDEVICE_H_
#define _IPHONEGRAPHICDEVICE_H_

#include <vector>
#include "athena/GraphicDevice.h"
#include "athena/Mesh.h"

namespace Athena
{
	class CIphoneGraphicDevice : public CGraphicDevice
	{
	public:
        static void                         CreateInstance(bool, const CVector2&);
        static void                         DestroyInstance();
        
		virtual void						Draw();
        
		virtual VertexBufferPtr				CreateVertexBuffer(const VERTEX_BUFFER_DESCRIPTOR&);
        
		virtual TexturePtr					CreateTextureFromFile(const char*);
		virtual TexturePtr					CreateTextureFromMemory(const void*, uint32);
		virtual TexturePtr					CreateTextureFromRawData(const void*, TEXTURE_FORMAT, uint32, uint32);
		
		virtual TexturePtr					CreateCubeTextureFromFile(const char*);
        
		virtual RenderTargetPtr				CreateRenderTarget(TEXTURE_FORMAT, uint32, uint32);
		virtual CubeRenderTargetPtr			CreateCubeRenderTarget(TEXTURE_FORMAT, uint32);
		
        void                                SetFrameRate(float);
        
	protected:
        typedef std::vector<CMesh*> RenderQueue;
        
                                            CIphoneGraphicDevice(bool, const CVector2&);
		virtual								~CIphoneGraphicDevice();
        
        bool                                FillRenderQueue(CSceneNode*, CCamera*);
        void                                DrawMesh(CMesh*);
        
        RenderQueue                         m_renderQueue;
        bool                                m_hasRetinaDisplay;
	};
}

#endif
