#include "palleon/win32/Dx11VertexBuffer.h"
#include <assert.h>
#include <vector>

using namespace Palleon;

CDx11VertexBuffer::CDx11VertexBuffer(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const VERTEX_BUFFER_DESCRIPTOR& descriptor)
: CVertexBuffer(descriptor)
, m_vertexBuffer(nullptr)
, m_indexBuffer(nullptr)
, m_deviceContext(deviceContext)
{
	HRESULT result = S_OK;

	uint32 vertexSize = descriptor.GetVertexSize();

	{
		D3D11_BUFFER_DESC bufferDesc = {};
		bufferDesc.BindFlags		= D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.ByteWidth		= vertexSize * descriptor.vertexCount;
		bufferDesc.Usage			= D3D11_USAGE_DYNAMIC;
		bufferDesc.CPUAccessFlags	= D3D11_CPU_ACCESS_WRITE;

		result = device->CreateBuffer(&bufferDesc, nullptr, &m_vertexBuffer);
		assert(SUCCEEDED(result));
	}

	{
		D3D11_BUFFER_DESC bufferDesc = {};
		bufferDesc.BindFlags		= D3D11_BIND_INDEX_BUFFER;
		bufferDesc.ByteWidth		= sizeof(uint16) * descriptor.indexCount;
		bufferDesc.Usage			= D3D11_USAGE_DYNAMIC;
		bufferDesc.CPUAccessFlags	= D3D11_CPU_ACCESS_WRITE;

		result = device->CreateBuffer(&bufferDesc, nullptr, &m_indexBuffer);
		assert(SUCCEEDED(result));
	}
}

CDx11VertexBuffer::~CDx11VertexBuffer()
{
	m_vertexBuffer->Release();
	m_indexBuffer->Release();
}

void CDx11VertexBuffer::UnlockVertices(uint32)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource = {};
	HRESULT result = m_deviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	assert(SUCCEEDED(result));
	memcpy(mappedResource.pData, m_shadowVertexBuffer, m_descriptor.GetVertexBufferSize());
	m_deviceContext->Unmap(m_vertexBuffer, 0);
}

void CDx11VertexBuffer::UnlockIndices()
{
	D3D11_MAPPED_SUBRESOURCE mappedResource = {};
	HRESULT result = m_deviceContext->Map(m_indexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	assert(SUCCEEDED(result));
	memcpy(mappedResource.pData, m_shadowIndexBuffer, m_descriptor.indexCount * sizeof(uint16));
	m_deviceContext->Unmap(m_indexBuffer, 0);
}

ID3D11Buffer* CDx11VertexBuffer::GetVertexBuffer() const
{
	return m_vertexBuffer;
}

ID3D11Buffer* CDx11VertexBuffer::GetIndexBuffer() const
{
	return m_indexBuffer;
}
