#ifndef _IPHONEGRAPHICDEVICE_H_
#define _IPHONEGRAPHICDEVICE_H_

#include "athena/GraphicDevice.h"

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
        
        void                                SetFrameRate(float);
        
	protected:
                                            CIphoneGraphicDevice(bool, const CVector2&);
		virtual								~CIphoneGraphicDevice();
        
        bool                                DrawNode(CSceneNode*);
        
        bool                                m_hasRetinaDisplay;
	};
}

#endif
