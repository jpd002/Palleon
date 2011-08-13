#include "athena/win32/Dx9VertexBuffer.h"
#include <assert.h>
#include <vector>

using namespace Athena;

CDx9VertexBuffer::CDx9VertexBuffer(IDirect3DDevice9* device, const VERTEX_BUFFER_DESCRIPTOR& descriptor, IDirect3DVertexDeclaration9* vertexDeclaration)
: CVertexBuffer(descriptor)
, m_vertexBuffer(NULL)
, m_indexBuffer(NULL)
, m_vertexDeclaration(vertexDeclaration)
{
	HRESULT result = S_OK;

	uint32 vertexSize = descriptor.GetVertexSize();
	result = device->CreateVertexBuffer(vertexSize * descriptor.vertexCount, D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT, &m_vertexBuffer, NULL);
	assert(SUCCEEDED(result));

	result = device->CreateIndexBuffer(sizeof(uint16) * descriptor.indexCount, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_indexBuffer, NULL);
	assert(SUCCEEDED(result));
}

CDx9VertexBuffer::~CDx9VertexBuffer()
{
	if(m_vertexBuffer != NULL)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = NULL;
	}

	if(m_indexBuffer != NULL)
	{
		m_indexBuffer->Release();
		m_indexBuffer = NULL;
	}
}

void* CDx9VertexBuffer::LockVertices()
{
	void* buffer(NULL);
	HRESULT result = m_vertexBuffer->Lock(0, 0, &buffer, 0);
	assert(SUCCEEDED(result));
	return buffer;
}

void CDx9VertexBuffer::UnlockVertices()
{
	HRESULT result = m_vertexBuffer->Unlock();
	assert(SUCCEEDED(result));
}

uint16* CDx9VertexBuffer::LockIndices()
{
	void* buffer(NULL);
	HRESULT result = m_indexBuffer->Lock(0, 0, &buffer, 0);
	assert(SUCCEEDED(result));
	return reinterpret_cast<uint16*>(buffer);
}

void CDx9VertexBuffer::UnlockIndices()
{
	HRESULT result = m_indexBuffer->Unlock();
	assert(SUCCEEDED(result));
}

IDirect3DVertexBuffer9* CDx9VertexBuffer::GetVertexBuffer() const
{
	return m_vertexBuffer;
}

IDirect3DIndexBuffer9* CDx9VertexBuffer::GetIndexBuffer() const
{
	return m_indexBuffer;
}

IDirect3DVertexDeclaration9* CDx9VertexBuffer::GetVertexDeclaration() const
{
	return m_vertexDeclaration;
}
