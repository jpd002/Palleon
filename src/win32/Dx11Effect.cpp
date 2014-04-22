#include "palleon/win32/Dx11Effect.h"
#include <D3Dcompiler.h>
#include <assert.h>

using namespace Palleon;

CDx11Effect::CDx11Effect(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
: m_device(device)
, m_deviceContext(deviceContext)
{

}

CDx11Effect::~CDx11Effect()
{

}

CDx11Effect::D3D11VertexShaderPtr CDx11Effect::GetVertexShader() const
{
	return m_vertexShader;
}

CDx11Effect::D3D11PixelShaderPtr CDx11Effect::GetPixelShader() const
{
	return m_pixelShader;
}

CDx11Effect::D3D11BufferPtr CDx11Effect::GetVertexConstantBuffer() const
{
	return m_vertexConstantBuffer;
}

CDx11Effect::D3D11BufferPtr CDx11Effect::GetPixelConstantBuffer() const
{
	return m_pixelConstantBuffer;
}

CDx11Effect::D3D11InputLayoutPtr CDx11Effect::GetInputLayout(const VERTEX_BUFFER_DESCRIPTOR& descriptor)
{
	auto inputLayoutIterator(m_inputLayouts.find(descriptor.vertexItems));
	if(inputLayoutIterator == std::end(m_inputLayouts))
	{
		auto inputLayout = CreateInputLayout(descriptor);
		m_inputLayouts.insert(std::make_pair(descriptor.vertexItems, inputLayout));
		return inputLayout;
	}
	else
	{
		return inputLayoutIterator->second;
	}
}

void CDx11Effect::CompileVertexShader(const std::string& shaderText)
{
	assert(m_vertexShader.IsEmpty());
	assert(m_vertexShaderCode.IsEmpty());

	Framework::Win32::CComPtr<ID3DBlob> vertexShaderCode;
	Framework::Win32::CComPtr<ID3DBlob> vertexShaderErrors;

	UINT compileFlags = 0;
#ifdef _DEBUG
	compileFlags |= D3DCOMPILE_DEBUG;
#endif

	HRESULT result = D3DCompile(shaderText.c_str(), shaderText.length() + 1, "vs", nullptr, nullptr, "VertexProgram", 
		"vs_5_0", compileFlags, 0, &vertexShaderCode, &vertexShaderErrors);
	if(FAILED(result))
	{
		if(!vertexShaderErrors.IsEmpty())
		{
			OutputDebugStringA(shaderText.c_str());
			OutputDebugStringA("\r\n");
			OutputDebugStringA(reinterpret_cast<const char*>(vertexShaderErrors->GetBufferPointer()));
		}
		DebugBreak();
	}

	result = m_device->CreateVertexShader(vertexShaderCode->GetBufferPointer(), vertexShaderCode->GetBufferSize(), nullptr, &m_vertexShader);
	assert(SUCCEEDED(result));

	m_vertexShaderCode = vertexShaderCode;
}

void CDx11Effect::CompilePixelShader(const std::string& shaderText)
{
	assert(m_pixelShader.IsEmpty());

	Framework::Win32::CComPtr<ID3DBlob> pixelShaderCode;
	Framework::Win32::CComPtr<ID3DBlob> pixelShaderErrors;

	UINT compileFlags = 0;
#ifdef _DEBUG
	compileFlags |= D3DCOMPILE_DEBUG;
#endif

	HRESULT result = D3DCompile(shaderText.c_str(), shaderText.length() + 1, "ps", nullptr, nullptr, "PixelProgram",
		"ps_5_0", compileFlags, 0, &pixelShaderCode, &pixelShaderErrors);
	if(FAILED(result))
	{
		if(!pixelShaderErrors.IsEmpty())
		{
			OutputDebugStringA(shaderText.c_str());
			OutputDebugStringA("\r\n");
			OutputDebugStringA(reinterpret_cast<const char*>(pixelShaderErrors->GetBufferPointer()));
		}
		DebugBreak();
	}

	result = m_device->CreatePixelShader(pixelShaderCode->GetBufferPointer(), pixelShaderCode->GetBufferSize(), nullptr, &m_pixelShader);
	assert(SUCCEEDED(result));
}

void CDx11Effect::CreateVertexConstantBuffer(uint32 size)
{
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.BindFlags		= D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.ByteWidth		= size;
	bufferDesc.Usage			= D3D11_USAGE_DYNAMIC;
	bufferDesc.CPUAccessFlags	= D3D11_CPU_ACCESS_WRITE;

	HRESULT result = m_device->CreateBuffer(&bufferDesc, nullptr, &m_vertexConstantBuffer);
	assert(SUCCEEDED(result));
}

void CDx11Effect::CreatePixelConstantBuffer(uint32 size)
{
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.BindFlags		= D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.ByteWidth		= size;
	bufferDesc.Usage			= D3D11_USAGE_DYNAMIC;
	bufferDesc.CPUAccessFlags	= D3D11_CPU_ACCESS_WRITE;

	HRESULT result = m_device->CreateBuffer(&bufferDesc, nullptr, &m_pixelConstantBuffer);
	assert(SUCCEEDED(result));
}
