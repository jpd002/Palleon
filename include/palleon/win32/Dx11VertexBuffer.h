#pragma once

#include <d3d11.h>
#include "../graphics/VertexBuffer.h"
#include "../win32/Dx11ContextManager.h"

namespace Palleon
{
	class CDx11VertexBuffer : public CVertexBuffer
	{
	public:
												CDx11VertexBuffer(ID3D11Device*, CDx11ContextManager&, const VERTEX_BUFFER_DESCRIPTOR&);
		virtual									~CDx11VertexBuffer();

		virtual void							UnlockVertices(uint32) override;
		virtual void							UnlockIndices() override;

		ID3D11Buffer*							GetVertexBuffer() const;
		ID3D11Buffer*							GetIndexBuffer() const;

	private:
		CDx11ContextManager&					m_contextManager;
		ID3D11Buffer*							m_vertexBuffer = nullptr;
		ID3D11Buffer*							m_indexBuffer = nullptr;
	};
}
