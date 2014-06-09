#include "palleon/ios/MetalVertexBuffer.h"

using namespace Palleon;

CMetalVertexBuffer::CMetalVertexBuffer(id<MTLDevice> device, const VERTEX_BUFFER_DESCRIPTOR& descriptor)
: CVertexBuffer(descriptor)
{
	uint32 vertexSize = descriptor.GetVertexSize();

	m_vertexBuffer = [device newBufferWithLength: vertexSize * descriptor.vertexCount options: MTLResourceOptionCPUCacheModeDefault];
	m_indexBuffer = [device newBufferWithLength: sizeof(uint16) * descriptor.indexCount options: MTLResourceOptionCPUCacheModeDefault];
}

CMetalVertexBuffer::~CMetalVertexBuffer()
{
	[m_vertexBuffer release];
	[m_indexBuffer release];
}

id<MTLBuffer> CMetalVertexBuffer::GetVertexBufferHandle() const
{
	return m_vertexBuffer;
}

id<MTLBuffer> CMetalVertexBuffer::GetIndexBufferHandle() const
{
	return m_indexBuffer;
}

void CMetalVertexBuffer::UnlockVertices(uint32)
{
	memcpy(m_vertexBuffer.contents, m_shadowVertexBuffer, m_descriptor.GetVertexBufferSize());
}

void CMetalVertexBuffer::UnlockIndices()
{
	memcpy(m_indexBuffer.contents, m_shadowIndexBuffer, m_descriptor.indexCount * sizeof(uint16));
}
