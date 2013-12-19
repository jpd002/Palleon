#include "athena/win32/Dx11Effect.h"
#include <D3Dcompiler.h>
#include <assert.h>

using namespace Athena;

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

CDx11Effect::D3D11BufferPtr CDx11Effect::GetConstantBuffer() const
{
	return m_constantBuffer;
}

CDx11Effect::D3D11InputLayoutPtr CDx11Effect::GetInputLayout(const VERTEX_BUFFER_DESCRIPTOR& descriptor)
{
	uint64 descriptorKey = descriptor.MakeKey();
	auto inputLayoutIterator(m_inputLayouts.find(descriptorKey));
	if(inputLayoutIterator == std::end(m_inputLayouts))
	{
		auto inputLayout = CreateInputLayout(descriptor);
		m_inputLayouts.insert(std::make_pair(descriptorKey, inputLayout));
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

	HRESULT result = D3DCompile(shaderText.c_str(), shaderText.length() + 1, "vs", nullptr, nullptr, "VertexProgram", 
		"vs_5_0", D3DCOMPILE_DEBUG, 0, &vertexShaderCode, &vertexShaderErrors);
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

	HRESULT result = D3DCompile(shaderText.c_str(), shaderText.length() + 1, "ps", nullptr, nullptr, "PixelProgram",
		"ps_5_0", D3DCOMPILE_DEBUG, 0, &pixelShaderCode, &pixelShaderErrors);
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

void CDx11Effect::CreateConstantBuffer(uint32 size)
{
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.BindFlags		= D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.ByteWidth		= size;
	bufferDesc.Usage			= D3D11_USAGE_DYNAMIC;
	bufferDesc.CPUAccessFlags	= D3D11_CPU_ACCESS_WRITE;

	HRESULT result = m_device->CreateBuffer(&bufferDesc, nullptr, &m_constantBuffer);
	assert(SUCCEEDED(result));
}

CDx11Effect::D3D11InputLayoutPtr CDx11Effect::CreateInputLayout(const VERTEX_BUFFER_DESCRIPTOR& descriptor)
{
	typedef std::vector<D3D11_INPUT_ELEMENT_DESC> InputElementArray;

	InputElementArray inputElements;

	uint32 vertexFlags = descriptor.vertexFlags;

	if(vertexFlags & VERTEX_BUFFER_HAS_POS)
	{
		D3D11_INPUT_ELEMENT_DESC inputElement = {};
		inputElement.SemanticName			= "POSITION";
		inputElement.SemanticIndex			= 0;
		inputElement.Format					= DXGI_FORMAT_R32G32B32_FLOAT;
		inputElement.AlignedByteOffset		= descriptor.posOffset;
		inputElement.InputSlotClass			= D3D11_INPUT_PER_VERTEX_DATA;
		inputElement.InstanceDataStepRate	= 0;
		inputElements.push_back(inputElement);
	}

	if(vertexFlags & VERTEX_BUFFER_HAS_NRM)
	{
		D3D11_INPUT_ELEMENT_DESC inputElement = {};
		inputElement.SemanticName			= "NORMAL";
		inputElement.SemanticIndex			= 0;
		inputElement.Format					= DXGI_FORMAT_R32G32B32_FLOAT;
		inputElement.AlignedByteOffset		= descriptor.nrmOffset;
		inputElement.InputSlotClass			= D3D11_INPUT_PER_VERTEX_DATA;
		inputElement.InstanceDataStepRate	= 0;
		inputElements.push_back(inputElement);
	}

	if(vertexFlags & VERTEX_BUFFER_HAS_UV0)
	{
		D3D11_INPUT_ELEMENT_DESC inputElement = {};
		inputElement.SemanticName			= "TEXCOORD";
		inputElement.SemanticIndex			= 0;
		inputElement.Format					= DXGI_FORMAT_R32G32_FLOAT;
		inputElement.AlignedByteOffset		= descriptor.uv0Offset;
		inputElement.InputSlotClass			= D3D11_INPUT_PER_VERTEX_DATA;
		inputElement.InstanceDataStepRate	= 0;
		inputElements.push_back(inputElement);
	}

	if(vertexFlags & VERTEX_BUFFER_HAS_UV1)
	{
		D3D11_INPUT_ELEMENT_DESC inputElement = {};
		inputElement.SemanticName			= "TEXCOORD";
		inputElement.SemanticIndex			= 1;
		inputElement.Format					= DXGI_FORMAT_R32G32_FLOAT;
		inputElement.AlignedByteOffset		= descriptor.uv1Offset;
		inputElement.InputSlotClass			= D3D11_INPUT_PER_VERTEX_DATA;
		inputElement.InstanceDataStepRate	= 0;
		inputElements.push_back(inputElement);
	}

	if(vertexFlags & VERTEX_BUFFER_HAS_COLOR)
	{
		D3D11_INPUT_ELEMENT_DESC inputElement = {};
		inputElement.SemanticName			= "COLOR";
		inputElement.SemanticIndex			= 0;
		inputElement.Format					= DXGI_FORMAT_R8G8B8A8_UNORM;
		inputElement.AlignedByteOffset		= descriptor.colorOffset;
		inputElement.InputSlotClass			= D3D11_INPUT_PER_VERTEX_DATA;
		inputElement.InstanceDataStepRate	= 0;
		inputElements.push_back(inputElement);
	}

	D3D11InputLayoutPtr inputLayout;
	HRESULT result = m_device->CreateInputLayout(inputElements.data(), inputElements.size(), 
		m_vertexShaderCode->GetBufferPointer(), m_vertexShaderCode->GetBufferSize(), &inputLayout);
	assert(SUCCEEDED(result));

	return inputLayout;
}
