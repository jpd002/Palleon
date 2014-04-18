#pragma once

#include "palleon/VertexBuffer.h"
#include "OpenGlEsDefs.h"

namespace Palleon
{
	class CGlEsVertexBuffer : public CVertexBuffer
	{
	public:
												CGlEsVertexBuffer(const VERTEX_BUFFER_DESCRIPTOR&, bool);
		virtual									~CGlEsVertexBuffer();

		virtual void							UnlockVertices(uint32) override;
		virtual void							UnlockIndices() override;

		void									BindBuffers() const;
		GLuint									GetVertexArray() const;
		
	protected:
		
		GLuint									m_vertexBuffer = -1;
		GLuint									m_indexBuffer = -1;
		GLuint									m_vertexArray = -1;
	};
}
