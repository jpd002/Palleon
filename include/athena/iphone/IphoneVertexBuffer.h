#ifndef _IPHONEVERTEXBUFFER_H_
#define _IPHONEVERTEXBUFFER_H_

#include "athena/VertexBuffer.h"
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>

namespace Athena
{
    class CIphoneVertexBuffer : public CVertexBuffer
    {
    public:
                                                CIphoneVertexBuffer(const VERTEX_BUFFER_DESCRIPTOR&);
		virtual									~CIphoneVertexBuffer();
        
		virtual void*							LockVertices();
		virtual void							UnlockVertices();
        
		virtual uint16*							LockIndices();
		virtual void							UnlockIndices();
        
        GLuint                                  GetVertexBuffer() const;
        uint16*                                 GetIndexBuffer() const;
        
    protected:
        GLuint                                  m_vertexBuffer;
        
        uint8*                                  m_shadowVertexBuffer;
        uint16*                                 m_indexBuffer;
    };
}

#endif
