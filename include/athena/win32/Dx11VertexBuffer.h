#pragma once

#include <d3d11.h>
#include "../VertexBuffer.h"

namespace Athena
{
	class CDx11VertexBuffer : public CVertexBuffer
	{
	public:
												CDx11VertexBuffer(ID3D11Device*, ID3D11DeviceContext*, const VERTEX_BUFFER_DESCRIPTOR&);
		virtual									~CDx11VertexBuffer();

		virtual void*							LockVertices();
		virtual void							UnlockVertices(uint32);

		virtual uint16*							LockIndices();
		virtual void							UnlockIndices();

		ID3D11Buffer*							GetVertexBuffer() const;
		ID3D11Buffer*							GetIndexBuffer() const;

	private:
		ID3D11DeviceContext*					m_deviceContext;
		ID3D11Buffer*							m_vertexBuffer;
		ID3D11Buffer*							m_indexBuffer;
	};
}
