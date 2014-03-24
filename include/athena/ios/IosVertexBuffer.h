#pragma once

#include "athena/VertexBuffer.h"
#include "OpenGlEsDefs.h"

namespace Athena
{
	class CIosVertexBuffer : public CVertexBuffer
	{
	public:
												CIosVertexBuffer(const VERTEX_BUFFER_DESCRIPTOR&);
		virtual									~CIosVertexBuffer();

		virtual void							UnlockVertices(uint32) override;
		virtual void							UnlockIndices() override;
		
		GLuint									GetVertexBuffer() const;
		GLuint									GetIndexBuffer() const;
		GLuint									GetVertexArray() const;
		
	protected:
		GLuint									m_vertexBuffer;
		GLuint									m_indexBuffer;
		GLuint									m_vertexArray;
	};
}
