#include "athena/Sprite.h"
#include "athena/GraphicDevice.h"

using namespace Athena;

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
: m_hotspot(0, 0)
, m_size(1, 1)
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
	return SpritePtr(new CSprite());
}

void CSprite::SetSize(const CVector2& size)
{
	m_size = size;
	m_status |= STATUS_VERTEXBUFFER_DIRTY;
}

void CSprite::SetHotspot(const CVector2& hotspot)
{
	m_hotspot = hotspot;
}

void CSprite::Update(float dt)
{
	CSceneNode::Update(dt);
	if(m_status & STATUS_VERTEXBUFFER_DIRTY)
	{
		UpdateVertices();
		m_status &= ~STATUS_VERTEXBUFFER_DIRTY;
	}
}

void CSprite::UpdateVertices()
{
	const VERTEX_BUFFER_DESCRIPTOR& bufferDesc(m_vertexBuffer->GetDescriptor());

	uint8* vertices = reinterpret_cast<uint8*>(m_vertexBuffer->LockVertices());
	for(unsigned int i = 0; i < 4; i++)
	{
		CVector3 position(&s_positions[i * 3]);
		position.x *= m_size.x;
		position.y *= m_size.y;

		*reinterpret_cast<CVector3*>(vertices + bufferDesc.posOffset) = position;
		*reinterpret_cast<CVector2*>(vertices + bufferDesc.uv0Offset) = CVector2(&s_texCoords[i * 2]);
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
