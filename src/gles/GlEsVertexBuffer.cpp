#include "athena/GlEs/GlEsVertexBuffer.h"
#include <assert.h>
#include <vector>

using namespace Athena;

CGlEsVertexBuffer::CGlEsVertexBuffer(const VERTEX_BUFFER_DESCRIPTOR& descriptor)
: CVertexBuffer(descriptor)
, m_vertexBuffer(0)
, m_indexBuffer(0)
, m_vertexArray(0)
{
	//Create vertex buffer
	{
		glGenBuffers(1, &m_vertexBuffer);
		CHECKGLERROR();
		
		glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
		CHECKGLERROR();
		
		glBufferData(GL_ARRAY_BUFFER, descriptor.GetVertexBufferSize(), NULL, GL_DYNAMIC_DRAW);
		CHECKGLERROR();
		
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECKGLERROR();
	}
	
	//Create index buffer
	{
		glGenBuffers(1, &m_indexBuffer);
		CHECKGLERROR();
		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
		CHECKGLERROR();
		
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16) * descriptor.indexCount, NULL, GL_DYNAMIC_DRAW);
		CHECKGLERROR();
		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		CHECKGLERROR();
	}
	
	//Create vertex array
	{
		glGenVertexArraysOES(1, &m_vertexArray);
		CHECKGLERROR();
		
		glBindVertexArrayOES(m_vertexArray);
		CHECKGLERROR();
		
		glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
		
		uint32 vertexSize = descriptor.GetVertexSize();

		for(const auto& vertexItem : descriptor.vertexItems)
		{
			if(vertexItem.id == VERTEX_ITEM_ID_NONE) continue;
			
			GLuint size = 0;
			GLenum type = GL_FLOAT;
			GLboolean normalized = GL_FALSE;
			switch(vertexItem.id)
			{
			case VERTEX_ITEM_ID_POSITION:
			case VERTEX_ITEM_ID_NORMAL:
				size = 3;
				type = GL_FLOAT;
				normalized = GL_FALSE;
				break;
			case VERTEX_ITEM_ID_UV0:
			case VERTEX_ITEM_ID_UV1:
				size = 2;
				type = GL_FLOAT;
				normalized = GL_FALSE;
				break;
			case VERTEX_ITEM_ID_COLOR:
				size = 4;
				type = GL_UNSIGNED_BYTE;
				normalized = GL_TRUE;
				break;
			default:
				size = vertexItem.size / sizeof(float);
				type = GL_FLOAT;
				normalized = GL_FALSE;
				break;
			}
			
			glEnableVertexAttribArray(vertexItem.id);
			glVertexAttribPointer(vertexItem.id, size, type, normalized, vertexSize, reinterpret_cast<const GLvoid*>(vertexItem.offset));
			CHECKGLERROR();
		}
		
		glBindVertexArrayOES(0);
	}
}

CGlEsVertexBuffer::~CGlEsVertexBuffer()
{
	glDeleteVertexArraysOES(1, &m_vertexArray);
	glDeleteBuffers(1, &m_vertexBuffer);
	glDeleteBuffers(1, &m_indexBuffer);
}

void CGlEsVertexBuffer::UnlockVertices(uint32 sizeHint)
{
	const auto& descriptor(GetDescriptor());
	
	if(sizeHint == 0)
	{
		sizeHint = descriptor.vertexCount;
	}
	
	uint32 vertexBufferSize = descriptor.GetVertexSize() * sizeHint;
	
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
	CHECKGLERROR();
	
	glBufferData(GL_ARRAY_BUFFER, vertexBufferSize, m_shadowVertexBuffer, GL_DYNAMIC_DRAW);
	CHECKGLERROR();
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	CHECKGLERROR();
}

void CGlEsVertexBuffer::UnlockIndices()
{
	const auto& descriptor = GetDescriptor();
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
	CHECKGLERROR();

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, descriptor.indexCount * sizeof(uint16), m_shadowIndexBuffer, GL_DYNAMIC_DRAW);
	CHECKGLERROR();
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	CHECKGLERROR();
}

GLuint CGlEsVertexBuffer::GetVertexBuffer() const
{
	return m_vertexBuffer;
}

GLuint CGlEsVertexBuffer::GetIndexBuffer() const
{
	return m_indexBuffer;
}

GLuint CGlEsVertexBuffer::GetVertexArray() const
{
	return m_vertexArray;
}