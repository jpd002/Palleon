#ifndef _VERTEXBUFFER_H_
#define _VERTEXBUFFER_H_

#include "Types.h"
#include "Vector3.h"
#include "Vector2.h"
#include <memory>

namespace Athena
{
	enum VERTEX_BUFFER_FLAGS
	{
		VERTEX_BUFFER_HAS_POS	= 0x01,
		VERTEX_BUFFER_HAS_NRM	= 0x02,
		VERTEX_BUFFER_HAS_COLOR	= 0x04,
		VERTEX_BUFFER_HAS_UV0	= 0x08,
		VERTEX_BUFFER_HAS_UV1	= 0x10,
	};

	struct VERTEX_BUFFER_DESCRIPTOR
	{
		uint32	vertexCount;
		uint32	indexCount;

		uint32	vertexFlags;
		uint32	posOffset;
		uint32	nrmOffset;
		uint32	colorOffset;
		uint32	uv0Offset;
		uint32	uv1Offset;

		uint32	GetVertexSize() const;

		uint64	MakeKey() const;
	};

	VERTEX_BUFFER_DESCRIPTOR				GenerateVertexBufferDescriptor(uint32 vertexCount, uint32 indexCount, uint32);

	class CVertexBuffer
	{
	public:
											CVertexBuffer(const VERTEX_BUFFER_DESCRIPTOR&);
		virtual								~CVertexBuffer();

		const VERTEX_BUFFER_DESCRIPTOR&		GetDescriptor() const;

		virtual void*						LockVertices() = 0;
		virtual void						UnlockVertices() = 0;

		virtual uint16*						LockIndices() = 0;
		virtual void						UnlockIndices() = 0;

	private:
		VERTEX_BUFFER_DESCRIPTOR			m_descriptor;
	};

	typedef std::tr1::shared_ptr<CVertexBuffer> VertexBufferPtr;
}

#endif
