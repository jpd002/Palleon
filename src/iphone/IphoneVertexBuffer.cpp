#include "IphoneVertexBuffer.h"
#include <assert.h>
#include <vector>

using namespace Athena;

CIphoneVertexBuffer::CIphoneVertexBuffer(const VERTEX_BUFFER_DESCRIPTOR& descriptor)
: CVertexBuffer(descriptor)
, m_vertexBuffer(NULL)
, m_shadowVertexBuffer(NULL)
, m_indexBuffer(NULL)
{
    uint32 vertexBufferSize = descriptor.GetVertexSize() * descriptor.vertexCount;
    m_shadowVertexBuffer = new uint8[vertexBufferSize];
    m_indexBuffer = new uint16[descriptor.indexCount];
    
    glGenBuffers(1, &m_vertexBuffer);
    assert(glGetError() == GL_NO_ERROR);
    
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    assert(glGetError() == GL_NO_ERROR);

    glBufferData(GL_ARRAY_BUFFER, vertexBufferSize, NULL, GL_STATIC_DRAW);
    assert(glGetError() == GL_NO_ERROR);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    assert(glGetError() == GL_NO_ERROR);
}

CIphoneVertexBuffer::~CIphoneVertexBuffer()
{
    delete [] m_shadowVertexBuffer;
    delete [] m_indexBuffer;
    glDeleteBuffers(1, &m_vertexBuffer);
}

void* CIphoneVertexBuffer::LockVertices()
{
    return m_shadowVertexBuffer;
}

void CIphoneVertexBuffer::UnlockVertices()
{
    const VERTEX_BUFFER_DESCRIPTOR& descriptor(GetDescriptor());
    
    uint32 vertexBufferSize = descriptor.GetVertexSize() * descriptor.vertexCount;
    
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
    assert(glGetError() == GL_NO_ERROR);
    
    glBufferData(GL_ARRAY_BUFFER, vertexBufferSize, m_shadowVertexBuffer, GL_STATIC_DRAW);
    assert(glGetError() == GL_NO_ERROR);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    assert(glGetError() == GL_NO_ERROR);
}

uint16* CIphoneVertexBuffer::LockIndices()
{
    return m_indexBuffer;
}

void CIphoneVertexBuffer::UnlockIndices()
{
    
}

GLuint CIphoneVertexBuffer::GetVertexBuffer() const
{
    return m_vertexBuffer;
}

uint16* CIphoneVertexBuffer::GetIndexBuffer() const
{
    return m_indexBuffer;
}
