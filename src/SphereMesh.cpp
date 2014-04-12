#include "palleon/SphereMesh.h"
#include "palleon/GraphicDevice.h"
#include "palleon/MathDef.h"

using namespace Palleon;

#define SPHERE_WIDTH	32
#define SPHERE_HEIGHT	16

CSphereMesh::CSphereMesh()
{
	unsigned int vertexCount = (SPHERE_HEIGHT - 2) * SPHERE_WIDTH + 2;
	unsigned int triangleCount = (SPHERE_HEIGHT - 2) * (SPHERE_WIDTH - 1) * 2;
	unsigned int indexCount = triangleCount * 3;

	VERTEX_BUFFER_DESCRIPTOR bufferDesc = GenerateVertexBufferDescriptor(vertexCount, indexCount, 
		VERTEX_BUFFER_HAS_POS | VERTEX_BUFFER_HAS_NRM);
	const auto& posVertexItem = bufferDesc.GetVertexItem(VERTEX_ITEM_ID_POSITION);
	const auto& nrmVertexItem = bufferDesc.GetVertexItem(VERTEX_ITEM_ID_NORMAL);

	m_primitiveType = PRIMITIVE_TRIANGLE_LIST;
	m_primitiveCount = triangleCount;
	m_boundingSphere.radius = 1;

	m_vertexBuffer = CGraphicDevice::GetInstance().CreateVertexBuffer(bufferDesc);

	uint8* vertices = reinterpret_cast<uint8*>(m_vertexBuffer->LockVertices());
	{
		for(unsigned int j = 1; j < SPHERE_HEIGHT - 1; j++)
		{
			for(unsigned int i = 0; i < SPHERE_WIDTH; i++)
			{
				float theta = static_cast<float>(j) / static_cast<float>(SPHERE_HEIGHT - 1) * M_PI;
				float phi = static_cast<float>(i) / static_cast<float>(SPHERE_WIDTH - 1) * M_PI * 2;

				CVector3 position;
				position.x =  sin(theta) * cos(phi);
				position.y =  cos(theta);
				position.z = -sin(theta) * sin(phi);

				*reinterpret_cast<CVector3*>(vertices + posVertexItem->offset) = position;
				*reinterpret_cast<CVector3*>(vertices + nrmVertexItem->offset) = position;

				vertices += bufferDesc.GetVertexSize();
			}
		}

		{
			CVector3 position(0, 1, 0);
			*reinterpret_cast<CVector3*>(vertices + posVertexItem->offset) = position;
			*reinterpret_cast<CVector3*>(vertices + nrmVertexItem->offset) = position;
			vertices += bufferDesc.GetVertexSize();
		}

		{
			CVector3 position(0, -1, 0);
			*reinterpret_cast<CVector3*>(vertices + posVertexItem->offset) = position;
			*reinterpret_cast<CVector3*>(vertices + nrmVertexItem->offset) = position;
			vertices += bufferDesc.GetVertexSize();
		}
	}
	m_vertexBuffer->UnlockVertices();

	uint16* indices = m_vertexBuffer->LockIndices();
	for(unsigned int j = 0; j < SPHERE_HEIGHT - 3; j++)
	{
		for(unsigned int i = 0; i < SPHERE_WIDTH - 1; i++)
		{
			(*indices++) = (j + 0) * SPHERE_WIDTH + (i + 0);
			(*indices++) = (j + 1) * SPHERE_WIDTH + (i + 1);
			(*indices++) = (j + 0) * SPHERE_WIDTH + (i + 1);
			(*indices++) = (j + 0) * SPHERE_WIDTH + (i + 0);
			(*indices++) = (j + 1) * SPHERE_WIDTH + (i + 0);
			(*indices++) = (j + 1) * SPHERE_WIDTH + (i + 1);
		}
	}
	for(unsigned int i = 0; i < SPHERE_WIDTH - 1; i++)
	{
		(*indices++) = (SPHERE_HEIGHT - 2) * SPHERE_WIDTH;
		(*indices++) = i;
		(*indices++) = i + 1;
		(*indices++) = (SPHERE_HEIGHT - 2) * SPHERE_WIDTH + 1;
		(*indices++) = (SPHERE_HEIGHT - 3) * SPHERE_WIDTH + i + 1;
		(*indices++) = (SPHERE_HEIGHT - 3) * SPHERE_WIDTH + i;
	}
	m_vertexBuffer->UnlockIndices();
}

CSphereMesh::~CSphereMesh()
{

}

MeshPtr CSphereMesh::Create()
{
	return MeshPtr(new CSphereMesh());
}
