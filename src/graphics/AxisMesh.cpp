#include "palleon/graphics/AxisMesh.h"
#include "palleon/graphics/GraphicDevice.h"

using namespace Palleon;

#define LINE_COUNT		3
#define VERTEX_COUNT	(LINE_COUNT * 2)
#define INDEX_COUNT		(LINE_COUNT * 2)

static const float s_positions[VERTEX_COUNT * 3] =
{
	0, 0, 0,
	1, 0, 0,
	0, 0, 0,
	0, 1, 0,
	0, 0, 0,
	0, 0, 1
};

static const float s_colors[VERTEX_COUNT * 4] =
{
	1, 0, 0, 1,
	1, 0, 0, 1,
	0, 1, 0, 1,
	0, 1, 0, 1,
	0, 0, 1, 1,
	0, 0, 1, 1
};

CAxisMesh::CAxisMesh()
{
	VERTEX_BUFFER_DESCRIPTOR bufferDesc = GenerateVertexBufferDescriptor(VERTEX_COUNT, INDEX_COUNT, 
		VERTEX_BUFFER_HAS_POS | VERTEX_BUFFER_HAS_COLOR);
	const auto& posVertexItem = bufferDesc.GetVertexItem(VERTEX_ITEM_ID_POSITION);
	const auto& colorVertexItem = bufferDesc.GetVertexItem(VERTEX_ITEM_ID_COLOR);

	m_primitiveType = PRIMITIVE_LINE_LIST;
	m_primitiveCount = LINE_COUNT;
	m_boundingSphere.radius = 1;

	m_vertexBuffer = CGraphicDevice::GetInstance().CreateVertexBuffer(bufferDesc);

	uint8* vertices = reinterpret_cast<uint8*>(m_vertexBuffer->LockVertices());
	{
		for(unsigned int i = 0; i < VERTEX_COUNT; i++)
		{
			*reinterpret_cast<CVector3*>(vertices + posVertexItem->offset) = CVector3(s_positions + i * 3);
			*reinterpret_cast<uint32*>(vertices + colorVertexItem->offset) = CGraphicDevice::ConvertColorToUInt32(CColor(s_colors + i * 4));

			vertices += bufferDesc.GetVertexSize();
		}
	}
	m_vertexBuffer->UnlockVertices();

	uint16* indices = m_vertexBuffer->LockIndices();
	for(unsigned int i = 0; i < INDEX_COUNT; i++)
	{
		(*indices++) = i;
	}
	m_vertexBuffer->UnlockIndices();
}

CAxisMesh::~CAxisMesh()
{

}

MeshPtr CAxisMesh::Create()
{
	return std::make_shared<CAxisMesh>();
}
