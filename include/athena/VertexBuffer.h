#pragma once

#include "Types.h"
#include "Vector3.h"
#include "Vector2.h"
#include <memory>
#include <array>
#include <functional>
#include <zlib.h>

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

	enum VERTEX_ITEM_ID
	{
		VERTEX_ITEM_ID_NONE,
		VERTEX_ITEM_ID_POSITION,
		VERTEX_ITEM_ID_NORMAL,
		VERTEX_ITEM_ID_COLOR,
		VERTEX_ITEM_ID_UV0,
		VERTEX_ITEM_ID_UV1,
		VERTEX_ITEM_ID_USER_START
	};

	struct VERTEX_ITEM
	{
		uint8		id = VERTEX_ITEM_ID_NONE;
		uint8		size = 0;
		uint16		offset = 0;
	};
	static_assert(sizeof(VERTEX_ITEM) == 4, "Size of VERTEX_ITEM must be 4 bytes.");

	enum
	{
		MAX_VERTEX_ITEMS = 8
	};

	typedef std::array<VERTEX_ITEM, MAX_VERTEX_ITEMS> VertexItemArray;

	struct VERTEX_ITEMS_KEY
	{
		VERTEX_ITEMS_KEY(const VertexItemArray& srcVertexItems) : vertexItems(srcVertexItems) { }

		bool operator ==(const VERTEX_ITEMS_KEY& rhs) const
		{
			return memcmp(vertexItems.data(), rhs.vertexItems.data(), sizeof(VERTEX_ITEM) * vertexItems.size()) == 0;
		}

		VertexItemArray		vertexItems;
	};
	
	struct VERTEX_BUFFER_DESCRIPTOR
	{
		uint32				vertexCount = 0;
		uint32				indexCount = 0;

		VertexItemArray		vertexItems;

		bool				HasVertexItem(uint32) const;
		const VERTEX_ITEM*	GetVertexItem(uint32) const;
		uint32				GetVertexSize() const;
	};

	VERTEX_BUFFER_DESCRIPTOR				GenerateVertexBufferDescriptor(uint32 vertexCount, uint32 indexCount, uint32);

	class CVertexBuffer
	{
	public:
											CVertexBuffer(const VERTEX_BUFFER_DESCRIPTOR&);
		virtual								~CVertexBuffer();

		const VERTEX_BUFFER_DESCRIPTOR&		GetDescriptor() const;

		virtual void*						LockVertices() = 0;
		virtual void						UnlockVertices(uint32 = 0) = 0;

		virtual uint16*						LockIndices() = 0;
		virtual void						UnlockIndices() = 0;

	private:
		VERTEX_BUFFER_DESCRIPTOR			m_descriptor;
	};

	typedef std::shared_ptr<CVertexBuffer> VertexBufferPtr;
}

namespace std
{
	template<>
	struct hash<Athena::VERTEX_ITEMS_KEY>
	{
		size_t operator ()(const Athena::VERTEX_ITEMS_KEY& itemsKey) const
		{
			uLong crc = crc32(0L, Z_NULL, 0);
			return crc32(crc, reinterpret_cast<const Bytef*>(itemsKey.vertexItems.data()), sizeof(Athena::VERTEX_ITEM) * itemsKey.vertexItems.size());
		}
	};
}

