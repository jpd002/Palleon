#include "IosVertexBuffer.h"
#include "IosGraphicDevice.h"
#include <assert.h>
#include <vector>

using namespace Athena;

CIosVertexBuffer::CIosVertexBuffer(const VERTEX_BUFFER_DESCRIPTOR& descriptor)
: CVertexBuffer(descriptor)
, m_vertexBuffer(NULL)
, m_vertexArray(NULL)
, m_shadowVertexBuffer(NULL)
, m_indexBuffer(NULL)
{
	uint32 vertexBufferSize = descriptor.GetVertexSize() * descriptor.vertexCount;
	m_shadowVertexBuffer = new uint8[vertexBufferSize];
	m_indexBuffer = new uint16[descriptor.indexCount];

	//Create vertex buffer
	{
		glGenBuffers(1, &m_vertexBuffer);
		CHECKGLERROR();
		
		glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
		CHECKGLERROR();
		
		glBufferData(GL_ARRAY_BUFFER, vertexBufferSize, NULL, GL_STATIC_DRAW);
		CHECKGLERROR();
		
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		CHECKGLERROR();
	}
	
	//Create vertex array
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
		
		glGenVertexArraysOES(1, &m_vertexArray);
		CHECKGLERROR();
		
		glBindVertexArrayOES(m_vertexArray);
		CHECKGLERROR();
		
		uint32 vertexSize = descriptor.GetVertexSize();
		
		assert(descriptor.vertexFlags & VERTEX_BUFFER_HAS_POS);
		
		glEnableVertexAttribArray(CIosGraphicDevice::VERTEX_ATTRIB_POSITION);
		glVertexAttribPointer(CIosGraphicDevice::VERTEX_ATTRIB_POSITION, 3, GL_FLOAT, GL_FALSE, vertexSize, reinterpret_cast<const GLvoid*>(descriptor.posOffset));
		CHECKGLERROR();
		
		if(descriptor.vertexFlags & VERTEX_BUFFER_HAS_COLOR)
		{
			glEnableVertexAttribArray(CIosGraphicDevice::VERTEX_ATTRIB_COLOR);
			glVertexAttribPointer(CIosGraphicDevice::VERTEX_ATTRIB_COLOR, 4, GL_UNSIGNED_BYTE, GL_TRUE, vertexSize, reinterpret_cast<const GLvoid*>(descriptor.colorOffset));
		}
		else
		{
			glDisableVertexAttribArray(CIosGraphicDevice::VERTEX_ATTRIB_COLOR);
		}
		CHECKGLERROR();
		
		if(descriptor.vertexFlags & VERTEX_BUFFER_HAS_UV0)
		{
			glEnableVertexAttribArray(CIosGraphicDevice::VERTEX_ATTRIB_TEXCOORD0);
			glVertexAttribPointer(CIosGraphicDevice::VERTEX_ATTRIB_TEXCOORD0, 2, GL_FLOAT, GL_FALSE, vertexSize, reinterpret_cast<const GLvoid*>(descriptor.uv0Offset));		
		}
		else
		{
			glDisableVertexAttribArray(CIosGraphicDevice::VERTEX_ATTRIB_TEXCOORD0);
		}
		CHECKGLERROR();
		
		if(descriptor.vertexFlags & VERTEX_BUFFER_HAS_UV1)
		{
			glEnableVertexAttribArray(CIosGraphicDevice::VERTEX_ATTRIB_TEXCOORD1);
			glVertexAttribPointer(CIosGraphicDevice::VERTEX_ATTRIB_TEXCOORD1, 2, GL_FLOAT, GL_FALSE, vertexSize, reinterpret_cast<const GLvoid*>(descriptor.uv1Offset));
		}
		else
		{
			glDisableVertexAttribArray(CIosGraphicDevice::VERTEX_ATTRIB_TEXCOORD1);
		}
		CHECKGLERROR();

		glBindVertexArrayOES(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}

CIosVertexBuffer::~CIosVertexBuffer()
{
	delete [] m_shadowVertexBuffer;
	delete [] m_indexBuffer;
	glDeleteVertexArraysOES(1, &m_vertexArray);
	glDeleteBuffers(1, &m_vertexBuffer);
}

void* CIosVertexBuffer::LockVertices()
{
	return m_shadowVertexBuffer;
}

void CIosVertexBuffer::UnlockVertices(uint32 sizeHint)
{
	const VERTEX_BUFFER_DESCRIPTOR& descriptor(GetDescriptor());
	
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

uint16* CIosVertexBuffer::LockIndices()
{
	return m_indexBuffer;
}

void CIosVertexBuffer::UnlockIndices()
{
	
}

GLuint CIosVertexBuffer::GetVertexBuffer() const
{
	return m_vertexBuffer;
}

uint16* CIosVertexBuffer::GetIndexBuffer() const
{
	return m_indexBuffer;
}

GLuint CIosVertexBuffer::GetVertexArray() const
{
	return m_vertexArray;
}
