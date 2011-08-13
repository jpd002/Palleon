#include <assert.h>
#include "athena/VertexBuffer.h"

using namespace Athena;

CVertexBuffer::CVertexBuffer(const VERTEX_BUFFER_DESCRIPTOR& descriptor)
: m_descriptor(descriptor)
{

}

CVertexBuffer::~CVertexBuffer()
{

}

const VERTEX_BUFFER_DESCRIPTOR& CVertexBuffer::GetDescriptor() const
{
	return m_descriptor;
}

VERTEX_BUFFER_DESCRIPTOR Athena::GenerateVertexBufferDescriptor(uint32 vertexCount, uint32 indexCount, uint32 vertexFlags)
{
	uint32 currentOffset = 0;

	VERTEX_BUFFER_DESCRIPTOR result;
	memset(&result, 0, sizeof(VERTEX_BUFFER_DESCRIPTOR));
	result.vertexCount	= vertexCount;
	result.indexCount	= indexCount;
	result.vertexFlags	= vertexFlags;

	if(vertexFlags & VERTEX_BUFFER_HAS_POS)
	{
		result.posOffset = currentOffset;
		currentOffset += sizeof(CVector3);
	}

	if(vertexFlags & VERTEX_BUFFER_HAS_UV0)
	{
		result.uv0Offset = currentOffset;
		currentOffset += sizeof(CVector2);
	}

	if(vertexFlags & VERTEX_BUFFER_HAS_UV1)
	{
		result.uv1Offset = currentOffset;
		currentOffset += sizeof(CVector2);
	}

	if(vertexFlags & VERTEX_BUFFER_HAS_COLOR)
	{
		result.colorOffset = currentOffset;
		currentOffset += sizeof(uint32);
	}

	return result;
}

uint32 Athena::VERTEX_BUFFER_DESCRIPTOR::GetVertexSize() const
{
	uint32 size = 0;
	if(vertexFlags & VERTEX_BUFFER_HAS_POS)
	{
		size += sizeof(CVector3);
	}
	if(vertexFlags & VERTEX_BUFFER_HAS_NRM)
	{
		size += sizeof(CVector3);
	}
	if(vertexFlags & VERTEX_BUFFER_HAS_COLOR)
	{
		size += sizeof(uint32);
	}
	if(vertexFlags & VERTEX_BUFFER_HAS_UV0)
	{
		size += sizeof(CVector2);
	}
	if(vertexFlags & VERTEX_BUFFER_HAS_UV1)
	{
		size += sizeof(CVector2);
	}
	return size;
}

uint64 Athena::VERTEX_BUFFER_DESCRIPTOR::MakeKey() const
{
	assert(vertexFlags < 0x100);
	assert(posOffset < 0x100);
	assert(nrmOffset < 0x100);
	assert(uv0Offset < 0x100);
	assert(uv1Offset < 0x100);
	assert(colorOffset < 0x100);
	return 
		(static_cast<uint64>(vertexFlags & 0xFF) <<  0) |
		(static_cast<uint64>(posOffset   & 0xFF) <<  8) |
		(static_cast<uint64>(nrmOffset   & 0xFF) << 16) |
		(static_cast<uint64>(colorOffset & 0xFF) << 24) |
		(static_cast<uint64>(uv0Offset   & 0xFF) << 32) |
		(static_cast<uint64>(uv1Offset   & 0xFF) << 40);
}
