#include "FullscreenQuad.h"
#include "palleon/graphics/GraphicDevice.h"
#include "palleon/PlatformDef.h"

using namespace Palleon;

static const int g_vertexCount = 3;

//x = (id / 2) * 4 - 1
//y = (id % 2) * 4 - 1
static const float s_positions[g_vertexCount * 3] =
{
	 3, -1, 0,
	-1, -1, 0,
	-1,  3, 0,
};

//u = (id / 2) * 2
//v = 1 - (id % 2) * 2
static const float s_texCoords[g_vertexCount * 2] =
{
#if defined(PALLEON_WIN32)
	2,  1,
	0,  1,
	0, -1,
#elif defined(PALLEON_IOS) || defined(PALLEON_ANDROID)
	2,  0,
	0,  0,
	0,  2,
#endif
};

CFullscreenQuad::CFullscreenQuad()
{
	auto bufferDesc = GenerateVertexBufferDescriptor(g_vertexCount, 3, VERTEX_BUFFER_HAS_POS | VERTEX_BUFFER_HAS_UV0);

	m_primitiveType = PRIMITIVE_TRIANGLE_LIST;
	m_primitiveCount = 1;
	m_vertexBuffer = CGraphicDevice::GetInstance().CreateVertexBuffer(bufferDesc);

	UpdateVertices();
	UpdateIndices();
}

CFullscreenQuad::~CFullscreenQuad()
{

}

MeshPtr CFullscreenQuad::Create()
{
	return std::make_shared<CFullscreenQuad>();
}

void CFullscreenQuad::UpdateVertices()
{
	const auto& bufferDesc(m_vertexBuffer->GetDescriptor());
	const auto& posVertexItem = bufferDesc.GetVertexItem(VERTEX_ITEM_ID_POSITION);
	const auto& uv0VertexItem = bufferDesc.GetVertexItem(VERTEX_ITEM_ID_UV0);

	uint8* vertices = reinterpret_cast<uint8*>(m_vertexBuffer->LockVertices());
	for(unsigned int i = 0; i < g_vertexCount; i++)
	{
		*reinterpret_cast<CVector3*>(vertices + posVertexItem->offset) = CVector3(&s_positions[i * 3]);
		*reinterpret_cast<CVector2*>(vertices + uv0VertexItem->offset) = CVector2(&s_texCoords[i * 2]);
		vertices += bufferDesc.GetVertexSize();
	}

	m_vertexBuffer->UnlockVertices();
}

void CFullscreenQuad::UpdateIndices()
{
	uint16* indices = m_vertexBuffer->LockIndices();
	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
	m_vertexBuffer->UnlockIndices();
}
