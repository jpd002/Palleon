#ifndef _BATCHMESH_H_
#define _BATCHMESH_H_

#include "PalleonEngine.h"

class CBatchMesh : public Palleon::CMesh
{
public:
										CBatchMesh(uint32, uint32, uint32);
	virtual								~CBatchMesh();
	
	void								BeginBatch();
	void								EndBatch();

	bool								CanWriteMesh(uint32 vertexCount, uint32 indexCount) const;

	void								WriteMesh(void* vertices, uint32 vertexCount, uint16* indices, uint32 indexCount);

private:
	uint32								m_currentVertex;
	uint32								m_currentIndex;
	uint8*								m_vertexBufferPtr;
	uint16*								m_indexBufferPtr;
	Palleon::VERTEX_BUFFER_DESCRIPTOR	m_bufferDesc;
#ifdef _DEBUG
	bool								m_batchBegun;
#endif
};

typedef std::shared_ptr<CBatchMesh> BatchMeshPtr;

#endif
