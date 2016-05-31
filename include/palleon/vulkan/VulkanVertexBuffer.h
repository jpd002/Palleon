#pragma once

#include "palleon/graphics/VertexBuffer.h"

namespace Palleon
{
	class CVulkanVertexBuffer : public CVertexBuffer
	{
	public:
		        CVulkanVertexBuffer(const VERTEX_BUFFER_DESCRIPTOR&);
		void    UnlockVertices(uint32) override;
		void    UnlockIndices() override;
	};
}
