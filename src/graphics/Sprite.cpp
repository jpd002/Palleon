#include "palleon/graphics/Sprite.h"
#include "palleon/graphics/GraphicDevice.h"

using namespace Palleon;

static const float s_positions[4 * 3] =
{
	0, 0, 0,
	1, 0, 0,
	0, 1, 0,
	1, 1, 0
};

static const float s_texCoords[4 * 2] =
{
	0, 0,
	1, 0,
	0, 1,
	1, 1
};

CSprite::CSprite()
: m_size(1, 1)
, m_status(0)
{
	VERTEX_BUFFER_DESCRIPTOR bufferDesc = GenerateVertexBufferDescriptor(4, 4, 
		VERTEX_BUFFER_HAS_POS | VERTEX_BUFFER_HAS_UV0);

	m_primitiveType = PRIMITIVE_TRIANGLE_STRIP;
	m_primitiveCount = 2;
	m_vertexBuffer = CGraphicDevice::GetInstance().CreateVertexBuffer(bufferDesc);

	UpdateIndices();
	m_status |= STATUS_VERTEXBUFFER_DIRTY;
}

CSprite::~CSprite()
{

}

SpritePtr CSprite::Create()
{
	return std::make_shared<CSprite>();
}

void CSprite::SetPosition(const CVector3& position)
{
	CMesh::SetPosition(position);
}

CVector2 CSprite::GetSize() const
{
	return m_size;
}

void CSprite::SetSize(const CVector2& size)
{
	m_size = size;
	m_status |= STATUS_VERTEXBUFFER_DIRTY;
}

void CSprite::Update(float dt)
{
	CMesh::Update(dt);
	if(m_status & STATUS_VERTEXBUFFER_DIRTY)
	{
		UpdateVertices();
		m_status &= ~STATUS_VERTEXBUFFER_DIRTY;
	}
}

void CSprite::UpdateVertices()
{
	const VERTEX_BUFFER_DESCRIPTOR& bufferDesc(m_vertexBuffer->GetDescriptor());
	const auto& posVertexItem = bufferDesc.GetVertexItem(VERTEX_ITEM_ID_POSITION);
	const auto& uv0VertexItem = bufferDesc.GetVertexItem(VERTEX_ITEM_ID_UV0);

	uint8* vertices = reinterpret_cast<uint8*>(m_vertexBuffer->LockVertices());
	for(unsigned int i = 0; i < 4; i++)
	{
		CVector3 position(&s_positions[i * 3]);
		position.x *= m_size.x;
		position.y *= m_size.y;

		*reinterpret_cast<CVector3*>(vertices + posVertexItem->offset) = position;
		*reinterpret_cast<CVector2*>(vertices + uv0VertexItem->offset) = CVector2(&s_texCoords[i * 2]);
		vertices += bufferDesc.GetVertexSize();
	}

	m_vertexBuffer->UnlockVertices();
}

void CSprite::UpdateIndices()
{
	uint16* indices = m_vertexBuffer->LockIndices();
	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
	indices[3] = 3;
	m_vertexBuffer->UnlockIndices();
}
