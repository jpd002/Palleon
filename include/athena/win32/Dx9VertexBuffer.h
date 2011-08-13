#ifndef _DX9VERTEXBUFFER_H_
#define _DX9VERTEXBUFFER_H_

#include <d3d9.h>
#include "../VertexBuffer.h"

namespace Athena
{
	class CDx9VertexBuffer : public CVertexBuffer
	{
	public:
												CDx9VertexBuffer(IDirect3DDevice9*, const VERTEX_BUFFER_DESCRIPTOR&, IDirect3DVertexDeclaration9*);
		virtual									~CDx9VertexBuffer();

		virtual void*							LockVertices();
		virtual void							UnlockVertices();

		virtual uint16*							LockIndices();
		virtual void							UnlockIndices();

		IDirect3DVertexBuffer9*					GetVertexBuffer() const;
		IDirect3DIndexBuffer9*					GetIndexBuffer() const;
		IDirect3DVertexDeclaration9*			GetVertexDeclaration() const;

	private:
		IDirect3DVertexBuffer9*					m_vertexBuffer;
		IDirect3DIndexBuffer9*					m_indexBuffer;
		IDirect3DVertexDeclaration9*			m_vertexDeclaration;
	};
}

#endif
