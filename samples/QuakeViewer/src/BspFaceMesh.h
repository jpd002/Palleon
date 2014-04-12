#ifndef _BSPMESH_H_
#define _BSPMESH_H_

#include "PalleonEngine.h"
#include "BspFile.h"
#include "BspMapResourceProvider.h"

class CBspFaceMesh;
typedef std::shared_ptr<CBspFaceMesh> BspFaceMeshPtr;

class CBspFaceMesh
{
public:
							CBspFaceMesh(const CBspFile&, unsigned int);
	virtual					~CBspFaceMesh();

	uint8*					GetVertices() const;
	uint16*					GetIndices() const;

	uint32					GetVertexCount() const;
	uint32					GetIndexCount() const;

protected:
	typedef std::vector<Bsp::VERTEX> VertexArray;
	typedef std::vector<uint16> IndexArray;

	void					BuildMeshOrPolygon(const CBspFile&, unsigned int);
	void					BuildPatch(const CBspFile&, unsigned int);

	void					Tessellate(const CBspFile&, unsigned int*, Bsp::VERTEX*, uint16, uint16*);

	uint8*					m_vertices;
	uint16*					m_indices;

	uint32					m_vertexCount;
	uint32					m_indexCount;
};

#endif
