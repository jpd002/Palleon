#include "palleon/GlEs/GlEsVertexBuffer.h"
#include <assert.h>
#include <vector>

using namespace Palleon;

CGlEsVertexBuffer::CGlEsVertexBuffer(const VERTEX_BUFFER_DESCRIPTOR& descriptor, bool useVertexArray)
: CVertexBuffer(descriptor)
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
	if(useVertexArray)
	{
#ifdef GL_ES_VERSION_3_0
		glGenVertexArrays(1, &m_vertexArray);
#else
		glGenVertexArraysOES(1, &m_vertexArray);
#endif
		CHECKGLERROR();
		
#ifdef GL_ES_VERSION_3_0
		glBindVertexArray(m_vertexArray);
#else
		glBindVertexArrayOES(m_vertexArray);
#endif
		CHECKGLERROR();
		
		BindBuffers();
		
#ifdef GL_ES_VERSION_3_0
		glBindVertexArray(0);
#else
		glBindVertexArrayOES(0);
#endif
	}
}

CGlEsVertexBuffer::~CGlEsVertexBuffer()
{
	if(m_vertexArray != -1)
	{
#ifdef GL_ES_VERSION_3_0
		glDeleteVertexArrays(1, &m_vertexArray);
#else
		glDeleteVertexArraysOES(1, &m_vertexArray);
#endif
	}
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

void CGlEsVertexBuffer::BindBuffers() const
{
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
	
	uint32 vertexSize = m_descriptor.GetVertexSize();
	
	for(const auto& vertexItem : m_descriptor.vertexItems)
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
}

GLuint CGlEsVertexBuffer::GetVertexArray() const
{
	return m_vertexArray;
}
