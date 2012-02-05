#include "athena/CubeMesh.h"
#include "athena/GraphicDevice.h"

using namespace Athena;

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

CCubeMesh::CCubeMesh()
{
	VERTEX_BUFFER_DESCRIPTOR bufferDesc = GenerateVertexBufferDescriptor(24, 36, 
		VERTEX_BUFFER_HAS_POS | VERTEX_BUFFER_HAS_UV0);

	m_primitiveType = PRIMITIVE_TRIANGLE_LIST;
	m_primitiveCount = 12;
	m_vertexBuffer = CGraphicDevice::GetInstance().CreateVertexBuffer(bufferDesc);

	uint8* vertices = reinterpret_cast<uint8*>(m_vertexBuffer->LockVertices());
	for(unsigned int i = 0; i < 24; i++)
	{
		*reinterpret_cast<CVector3*>(vertices + bufferDesc.posOffset) = CVector3(&s_positions[i * 3]);
		*reinterpret_cast<CVector2*>(vertices + bufferDesc.uv0Offset) = CVector2(&s_texCoords[i * 2]);
		vertices += bufferDesc.GetVertexSize();
	}
	m_vertexBuffer->UnlockVertices();

	uint16* indices = m_vertexBuffer->LockIndices();
	for(unsigned int i = 0; i < 6; i++)
	{
		indices[(i * 6) + 0] = (i * 4) + 1;
		indices[(i * 6) + 1] = (i * 4) + 0;
		indices[(i * 6) + 2] = (i * 4) + 2;
		indices[(i * 6) + 3] = (i * 4) + 1;
		indices[(i * 6) + 4] = (i * 4) + 2;
		indices[(i * 6) + 5] = (i * 4) + 3;
	}
	m_vertexBuffer->UnlockIndices();
}

CCubeMesh::~CCubeMesh()
{

}

MeshPtr CCubeMesh::Create()
{
	return MeshPtr(new CCubeMesh());
}
