#include <assert.h>
#include "athena/VertexBuffer.h"

using namespace Athena;

CVertexBuffer::CVertexBuffer(const VERTEX_BUFFER_DESCRIPTOR& descriptor)
: m_descriptor(descriptor)
{
	m_shadowVertexBuffer = new uint8[descriptor.GetVertexBufferSize()];
	m_shadowIndexBuffer = new uint16[descriptor.indexCount];
}

CVertexBuffer::~CVertexBuffer()
{
	delete [] m_shadowVertexBuffer;
	delete [] m_shadowIndexBuffer;
}

const VERTEX_BUFFER_DESCRIPTOR& CVertexBuffer::GetDescriptor() const
{
	return m_descriptor;
}

const void* CVertexBuffer::GetShadowVertexBuffer() const
{
	return m_shadowVertexBuffer;
}

const uint16* CVertexBuffer::GetShadowIndexBuffer() const
{
	return m_shadowIndexBuffer;
}

void* CVertexBuffer::LockVertices()
{
	return m_shadowVertexBuffer;
}

uint16* CVertexBuffer::LockIndices()
{
	return m_shadowIndexBuffer;
}

VERTEX_BUFFER_DESCRIPTOR Athena::GenerateVertexBufferDescriptor(uint32 vertexCount, uint32 indexCount, uint32 vertexFlags)
{
	uint32 currentOffset = 0;
	unsigned int currentVertexItem = 0;

	VERTEX_BUFFER_DESCRIPTOR result;
	result.vertexCount	= vertexCount;
	result.indexCount	= indexCount;

	if(vertexFlags & VERTEX_BUFFER_HAS_POS)
	{
		auto& vertexItem = result.vertexItems[currentVertexItem++];
		vertexItem.id = VERTEX_ITEM_ID_POSITION;
		vertexItem.offset = currentOffset;
		vertexItem.size = sizeof(CVector3);
		currentOffset += vertexItem.size;
	}

	if(vertexFlags & VERTEX_BUFFER_HAS_NRM)
	{
		auto& vertexItem = result.vertexItems[currentVertexItem++];
		vertexItem.id = VERTEX_ITEM_ID_NORMAL;
		vertexItem.offset = currentOffset;
		vertexItem.size = sizeof(CVector3);
		currentOffset += vertexItem.size;
	}

	if(vertexFlags & VERTEX_BUFFER_HAS_UV0)
	{
		auto& vertexItem = result.vertexItems[currentVertexItem++];
		vertexItem.id = VERTEX_ITEM_ID_UV0;
		vertexItem.offset = currentOffset;
		vertexItem.size = sizeof(CVector2);
		currentOffset += vertexItem.size;
	}

	if(vertexFlags & VERTEX_BUFFER_HAS_UV1)
	{
		auto& vertexItem = result.vertexItems[currentVertexItem++];
		vertexItem.id = VERTEX_ITEM_ID_UV1;
		vertexItem.offset = currentOffset;
		vertexItem.size = sizeof(CVector2);
		currentOffset += vertexItem.size;
	}

	if(vertexFlags & VERTEX_BUFFER_HAS_COLOR)
	{
		auto& vertexItem = result.vertexItems[currentVertexItem++];
		vertexItem.id = VERTEX_ITEM_ID_COLOR;
		vertexItem.offset = currentOffset;
		vertexItem.size = sizeof(uint32);
		currentOffset += vertexItem.size;
	}

	return result;
}

const Athena::VERTEX_ITEM* Athena::VERTEX_BUFFER_DESCRIPTOR::GetVertexItem(uint32 id) const
{
	for(const auto& vertexItem : vertexItems)
	{
		if(vertexItem.id == id)
		{
			return &vertexItem;
		}
	}
	return nullptr;
}

bool Athena::VERTEX_BUFFER_DESCRIPTOR::HasVertexItem(uint32 id) const
{
	return GetVertexItem(id) != nullptr;
}

uint32 Athena::VERTEX_BUFFER_DESCRIPTOR::GetVertexSize() const
{
	uint32 size = 0;
	for(const auto& vertexItem : vertexItems)
	{
		size += vertexItem.size;
	}
	return size;
}

uint32 Athena::VERTEX_BUFFER_DESCRIPTOR::GetVertexBufferSize() const
{
	return GetVertexSize() * vertexCount;
}
