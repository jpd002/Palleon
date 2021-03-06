#include "palleon/graphics/NinePatch.h"
#include "palleon/graphics/GraphicDevice.h"

using namespace Palleon;

#define PRIMITIVE_COUNT 18
#define VERTEX_COUNT (16)
#define INDEX_COUNT (PRIMITIVE_COUNT * 3)

static const uint16 s_indices[INDEX_COUNT] =
{
	0, 5, 4,
	0, 1, 5,

	1, 6, 5,
	1, 2, 6,

	2, 7, 6,
	2, 3, 7,

	4, 9, 8,
	4, 5, 9,

	5, 10, 9,
	5, 6, 10,

	6, 11, 10,
	6, 7, 11,

	8, 13, 12,
	8, 9, 13,

	9, 14, 13,
	9, 10, 14,

	10, 15, 14,
	10, 11, 15,
};

CNinePatch::CNinePatch()
{
	auto bufferDesc = GenerateVertexBufferDescriptor(VERTEX_COUNT, INDEX_COUNT, 
		VERTEX_BUFFER_HAS_POS | VERTEX_BUFFER_HAS_UV0);

	m_primitiveType = PRIMITIVE_TRIANGLE_LIST;
	m_primitiveCount = PRIMITIVE_COUNT;
	m_vertexBuffer = CGraphicDevice::GetInstance().CreateVertexBuffer(bufferDesc);

	UpdateIndices();
	m_status |= STATUS_VERTEXBUFFER_DIRTY;
}

CNinePatch::~CNinePatch()
{

}

NinePatchPtr CNinePatch::Create()
{
	return NinePatchPtr(new CNinePatch());
}

void CNinePatch::SetSize(const CVector2& size)
{
	m_size = size;
	m_status |= STATUS_VERTEXBUFFER_DIRTY;
}

void CNinePatch::SetDescriptor(const CNinePatchDescriptor* descriptor)
{
	assert(descriptor != nullptr);
	m_descriptor = descriptor;
	m_material->SetTexture(0, Palleon::CResourceManager::GetInstance().GetTexture(m_descriptor->GetTextureName()));
	m_material->SetAlphaBlendingMode(Palleon::ALPHA_BLENDING_LERP);
	m_status |= STATUS_VERTEXBUFFER_DIRTY;
}

void CNinePatch::Update(float dt)
{
	CMesh::Update(dt);
	if(m_status & STATUS_VERTEXBUFFER_DIRTY)
	{
		UpdateVertices();
		m_status &= ~STATUS_VERTEXBUFFER_DIRTY;
	}
}

void CNinePatch::UpdateVertices()
{
	if(!m_descriptor)
	{
		return;
	}

	const VERTEX_BUFFER_DESCRIPTOR& bufferDesc(m_vertexBuffer->GetDescriptor());
	const auto& posVertexItem = bufferDesc.GetVertexItem(VERTEX_ITEM_ID_POSITION);
	const auto& uv0VertexItem = bufferDesc.GetVertexItem(VERTEX_ITEM_ID_UV0);

	float posX[4] =
	{
		0,
		static_cast<float>(m_descriptor->GetLeftMargin()),
		m_size.x - m_descriptor->GetRightMargin(),
		m_size.x
	};

	float posY[4] =
	{
		0,
		static_cast<float>(m_descriptor->GetTopMargin()),
		m_size.y - m_descriptor->GetBottomMargin(),
		m_size.y
	};

	float texU[4] =
	{
		0,
		static_cast<float>(m_descriptor->GetLeftMargin()),
		static_cast<float>(m_descriptor->GetWidth() - m_descriptor->GetRightMargin()),
		static_cast<float>(m_descriptor->GetWidth()),
	};

	float texV[4] =
	{
		0,
		static_cast<float>(m_descriptor->GetTopMargin()),
		static_cast<float>(m_descriptor->GetHeight() - m_descriptor->GetBottomMargin()),
		static_cast<float>(m_descriptor->GetHeight()),
	};

	uint8* vertices = reinterpret_cast<uint8*>(m_vertexBuffer->LockVertices());
	for(unsigned int y = 0; y < 4; y++)
	{
		for(unsigned int x = 0; x < 4; x++)
		{
			CVector3 position(posX[x], posY[y], 0);
			CVector2 texCoord(
				texU[x] / static_cast<float>(m_descriptor->GetWidth()), 
				texV[y] / static_cast<float>(m_descriptor->GetHeight())
				);
			*reinterpret_cast<CVector3*>(vertices + posVertexItem->offset) = position;
			*reinterpret_cast<CVector2*>(vertices + uv0VertexItem->offset) = texCoord;
			vertices += bufferDesc.GetVertexSize();
		}
	}
	m_vertexBuffer->UnlockVertices();
}

void CNinePatch::UpdateIndices()
{
	uint16* indices = m_vertexBuffer->LockIndices();
	memcpy(indices, s_indices, sizeof(s_indices));
	m_vertexBuffer->UnlockIndices();
}
