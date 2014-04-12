#include "palleon/ConeMesh.h"
#include "palleon/GraphicDevice.h"

using namespace Palleon;

#define RADIAL_PARTS	(32)

CConeMesh::CConeMesh()
{
	unsigned int vertexCount = RADIAL_PARTS + 2;
	unsigned int triangleCount = RADIAL_PARTS * 2;
	unsigned int indexCount = triangleCount * 3;

	VERTEX_BUFFER_DESCRIPTOR bufferDesc = GenerateVertexBufferDescriptor(vertexCount, indexCount, 
		VERTEX_BUFFER_HAS_POS | VERTEX_BUFFER_HAS_NRM);
	const auto& posVertexItem = bufferDesc.GetVertexItem(VERTEX_ITEM_ID_POSITION);
	const auto& nrmVertexItem = bufferDesc.GetVertexItem(VERTEX_ITEM_ID_NORMAL);

	m_primitiveType = PRIMITIVE_TRIANGLE_LIST;
	m_primitiveCount = triangleCount;
	m_boundingSphere.radius = sqrt(2.f);

	m_vertexBuffer = CGraphicDevice::GetInstance().CreateVertexBuffer(bufferDesc);

	uint8* vertices = reinterpret_cast<uint8*>(m_vertexBuffer->LockVertices());
	{
		//Bottom point
		{
			*reinterpret_cast<CVector3*>(vertices + posVertexItem->offset) = CVector3(0, 0, 0);
			*reinterpret_cast<CVector3*>(vertices + nrmVertexItem->offset) = CVector3(0, -1, 0);

			vertices += bufferDesc.GetVertexSize();
		}

		//Top point
		{
			*reinterpret_cast<CVector3*>(vertices + posVertexItem->offset) = CVector3(0, 1, 0);
			*reinterpret_cast<CVector3*>(vertices + nrmVertexItem->offset) = CVector3(0, 1, 0);

			vertices += bufferDesc.GetVertexSize();
		}

		for(unsigned int i = 0; i < RADIAL_PARTS; i++)
		{
			float theta = static_cast<float>(i) / static_cast<float>(RADIAL_PARTS) * M_PI * 2;

			CVector3 position(cos(theta), 0, sin(theta));
			CVector3 normal(cos(theta), 0, sin(theta));

			*reinterpret_cast<CVector3*>(vertices + posVertexItem->offset) = position;
			*reinterpret_cast<CVector3*>(vertices + nrmVertexItem->offset) = normal;

			vertices += bufferDesc.GetVertexSize();
		}
	}
	m_vertexBuffer->UnlockVertices();

	uint16* indices = m_vertexBuffer->LockIndices();
	//Actual Cone Part
	for(unsigned int i = 0; i < RADIAL_PARTS; i++)
	{
		unsigned int currentIndex = i;
		unsigned int nextIndex = (i + 1) % RADIAL_PARTS;
		(*indices++) = 0;
		(*indices++) = 2 + currentIndex;
		(*indices++) = 2 + nextIndex;
	}
	//Bottom Cap
	for(unsigned int i = 0; i < RADIAL_PARTS; i++)
	{
		unsigned int currentIndex = i;
		unsigned int nextIndex = (i + 1) % RADIAL_PARTS;
		(*indices++) = 2 + currentIndex;
		(*indices++) = 1;
		(*indices++) = 2 + nextIndex;
	}
	m_vertexBuffer->UnlockIndices();
}

CConeMesh::~CConeMesh()
{

}

MeshPtr CConeMesh::Create()
{
	return std::make_shared<CConeMesh>();
}
