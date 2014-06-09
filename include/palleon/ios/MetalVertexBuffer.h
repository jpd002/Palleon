#pragma once

#include <Metal/Metal.h>
#include "palleon/VertexBuffer.h"

namespace Palleon
{
	class CMetalVertexBuffer : public CVertexBuffer
	{
	public:
									CMetalVertexBuffer(id<MTLDevice>, const VERTEX_BUFFER_DESCRIPTOR&);
		virtual						~CMetalVertexBuffer();
		
		id<MTLBuffer>				GetVertexBufferHandle() const;
		id<MTLBuffer>				GetIndexBufferHandle() const;
		
		virtual void				UnlockVertices(uint32) override;
		virtual void				UnlockIndices() override;
		
	private:
		id<MTLBuffer>				m_vertexBuffer;
		id<MTLBuffer>				m_indexBuffer;
	};
}
