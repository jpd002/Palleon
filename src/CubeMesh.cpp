#include "palleon/CubeMesh.h"
#include "palleon/GraphicDevice.h"

using namespace Palleon;

static const float s_positions[24 * 3] =
{
	//Front
	-1,  1, -1,
	-1, -1, -1,
	 1,  1, -1,
	 1, -1, -1,
	 //Back
	 1, -1,  1,
	-1, -1,  1,
	 1,  1,  1,
	-1,  1,  1,
	 //Left
	-1, -1,  1,
	-1, -1, -1,
	-1,  1,  1,
	-1,  1, -1,
	 //Right
	 1,  1, -1,
	 1, -1, -1,
	 1,  1,  1,
	 1, -1,  1,
	//Bottom
	 1, -1, -1,
	-1, -1, -1,
	 1, -1,  1,
	-1, -1,  1,
	//Top
	-1,  1,  1,
	-1,  1, -1,
	 1,  1,  1,
	 1,  1, -1,
};

static const float s_texCoords[24 * 2] =
{
	//Front
	0,  0,
	0,  1,
	1,  0,
	1,  1,
	//Back
	0,  1,
	1,  1,
	0,  0,
	1,  0,
	//Left
	0,  1,
	1,  1,
	0,  0,
	1,  0,
	//Right
	0,  0,
	0,  1,
	1,  0,
	1,  1,
	//Bottom
	0,  1,
	1,  1,
	0,  0,
	1,  0,
	//Top
	0,  0,
	0,  1,
	1,  0,
	1,  1,
};

#define GENERATE_FACE_INDICES(i) (i * 4) + 1, (i * 4) + 0, (i * 4) + 2, (i * 4) + 1, (i * 4) + 2, (i * 4) + 3

static const uint16 s_indices[36] =
{
	GENERATE_FACE_INDICES(0),
	GENERATE_FACE_INDICES(1),
	GENERATE_FACE_INDICES(2),
	GENERATE_FACE_INDICES(3),
	GENERATE_FACE_INDICES(4),
	GENERATE_FACE_INDICES(5)
};

CCubeMesh::CCubeMesh()
{
	VERTEX_BUFFER_DESCRIPTOR bufferDesc = GenerateVertexBufferDescriptor(24, 36, 
		VERTEX_BUFFER_HAS_POS | VERTEX_BUFFER_HAS_UV0);
	const auto& posVertexItem = bufferDesc.GetVertexItem(VERTEX_ITEM_ID_POSITION);
	const auto& uv0VertexItem = bufferDesc.GetVertexItem(VERTEX_ITEM_ID_UV0);

	m_primitiveType = PRIMITIVE_TRIANGLE_LIST;
	m_primitiveCount = 12;
	m_vertexBuffer = CGraphicDevice::GetInstance().CreateVertexBuffer(bufferDesc);

	uint8* vertices = reinterpret_cast<uint8*>(m_vertexBuffer->LockVertices());
	for(unsigned int i = 0; i < 24; i++)
	{
		*reinterpret_cast<CVector3*>(vertices + posVertexItem->offset) = CVector3(&s_positions[i * 3]);
		*reinterpret_cast<CVector2*>(vertices + uv0VertexItem->offset) = CVector2(&s_texCoords[i * 2]);
		vertices += bufferDesc.GetVertexSize();
	}
	m_vertexBuffer->UnlockVertices();

	uint16* indices = m_vertexBuffer->LockIndices();
	memcpy(indices, s_indices, sizeof(s_indices));
	m_vertexBuffer->UnlockIndices();
}

CCubeMesh::~CCubeMesh()
{

}

MeshPtr CCubeMesh::Create()
{
	return MeshPtr(new CCubeMesh());
}
