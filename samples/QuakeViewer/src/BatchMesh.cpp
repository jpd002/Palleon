#include "BatchMesh.h"

CBatchMesh::CBatchMesh(uint32 vertexCount, uint32 indexCount, uint32 vertexFlags)
: m_currentVertex(0)
, m_currentIndex(0)
#ifdef _DEBUG
, m_batchBegun(false)
#endif
{
	//Create vertex buffer
	m_bufferDesc = Athena::GenerateVertexBufferDescriptor(vertexCount, indexCount, vertexFlags);

	m_vertexBuffer = Athena::CGraphicDevice::GetInstance().CreateVertexBuffer(m_bufferDesc);
	m_primitiveType = Athena::PRIMITIVE_TRIANGLE_LIST;
	m_primitiveCount = 0;
}

CBatchMesh::~CBatchMesh()
{

}

void CBatchMesh::BeginBatch()
{
#ifdef _DEBUG
	assert(!m_batchBegun);
	m_batchBegun = true;
#endif

	m_currentVertex = 0;
	m_currentIndex = 0;
	m_vertexBufferPtr = reinterpret_cast<uint8*>(m_vertexBuffer->LockVertices());
	m_indexBufferPtr = m_vertexBuffer->LockIndices();
}

void CBatchMesh::EndBatch()
{
#ifdef _DEBUG
	assert(m_batchBegun);
	m_batchBegun = false;
#endif

	m_vertexBuffer->UnlockVertices(m_currentVertex);
	m_vertexBuffer->UnlockIndices();

	assert((m_currentIndex % 3) == 0);
	m_primitiveCount = m_currentIndex / 3;
}

void CBatchMesh::WriteMesh(void* vertices, uint32 vertexCount, uint16* indices, uint32 indexCount)
{
#ifdef _DEBUG
	assert(m_batchBegun);
#endif

	uint32 batchVertexSize = vertexCount * m_bufferDesc.GetVertexSize();
	memcpy(m_vertexBufferPtr, vertices, batchVertexSize);
	m_vertexBufferPtr += batchVertexSize;

	//Check index overflow
	assert((m_currentVertex + vertexCount) < 0x10000);
	for(unsigned int i = 0; i < indexCount; i++)
	{
		(*m_indexBufferPtr) = static_cast<uint16>(indices[i] + m_currentVertex);
		m_indexBufferPtr++;
	}

	m_currentIndex += indexCount;
	m_currentVertex += vertexCount;
}

bool CBatchMesh::CanWriteMesh(uint32 vertexCount, uint32 indexCount) const
{
	return 
		((m_bufferDesc.vertexCount - m_currentVertex) >= vertexCount) &&
		((m_bufferDesc.indexCount - m_currentIndex) >= indexCount);
}
