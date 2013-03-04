#pragma once

#include "athena/VertexBuffer.h"
#include "OpenGlEsDefs.h"

namespace Athena
{
	class CIphoneVertexBuffer : public CVertexBuffer
	{
	public:
												CIphoneVertexBuffer(const VERTEX_BUFFER_DESCRIPTOR&);
		virtual									~CIphoneVertexBuffer();

		virtual void*							LockVertices();
		virtual void							UnlockVertices(uint32);
		
		virtual uint16*							LockIndices();
		virtual void							UnlockIndices();
		
		GLuint									GetVertexBuffer() const;
		uint16*									GetIndexBuffer() const;
		GLuint									GetVertexArray() const;
		
	protected:
		GLuint									m_vertexBuffer;
		GLuint									m_vertexArray;
		
		uint8*									m_shadowVertexBuffer;
		uint16*									m_indexBuffer;
	};
}
