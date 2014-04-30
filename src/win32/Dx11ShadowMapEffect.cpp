#include "palleon/win32/Dx11ShadowMapEffect.h"

using namespace Palleon;

CDx11ShadowMapEffect::CDx11ShadowMapEffect(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
: CDx11Effect(device, deviceContext)
{
	static const char* g_vertexShader =
	{
		"cbuffer MatrixBuffer"
		"{"
		"matrix c_worldMatrix;"
		"matrix c_viewProjMatrix;"
		"};"
		"struct VertexInputType"
		"{"
		"float3 position : POSITION;"
		"};"
		"struct PixelInputType"
		"{"
		"float4 position : SV_POSITION;"
		"};"
		"PixelInputType VertexProgram(VertexInputType input)"
		"{"
		"PixelInputType output;"
		"output.position = mul(c_worldMatrix, float4(input.position, 1));"
		"output.position = mul(c_viewProjMatrix, output.position);"
		"return output;"
		"}"
	};

	static const char* g_pixelShader =
	{
		"struct PixelInputType"
		"{"
		"float4 position : SV_POSITION;"
		"};"
		"float4 PixelProgram(PixelInputType input) : SV_TARGET"
		"{"
		"return float4(input.position.z, 0, 0, 0);"
		"}"
	};

	CompileVertexShader(g_vertexShader);
	CompilePixelShader(g_pixelShader);

	OffsetKeeper constantOffset;

	m_worldMatrixOffset				= constantOffset.Allocate(0x40);
	m_viewProjMatrixOffset			= constantOffset.Allocate(0x40);
	
	CreateVertexConstantBuffer(constantOffset.currentOffset);
}

CDx11ShadowMapEffect::~CDx11ShadowMapEffect()
{

}

void CDx11ShadowMapEffect::UpdateConstants(const DX11VIEWPORT_PARAMS& viewportParams, CMaterial* material, const CMatrix4& worldMatrix)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource = {};
	HRESULT result = m_deviceContext->Map(m_vertexConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	assert(SUCCEEDED(result));

	auto constantBufferPtr = reinterpret_cast<uint8*>(mappedResource.pData);
	*reinterpret_cast<CMatrix4*>(constantBufferPtr + m_worldMatrixOffset) = worldMatrix;
	*reinterpret_cast<CMatrix4*>(constantBufferPtr + m_viewProjMatrixOffset) = viewportParams.viewMatrix * viewportParams.projMatrix;

	m_deviceContext->Unmap(m_vertexConstantBuffer, 0);
}

CDx11Effect::D3D11InputLayoutPtr CDx11ShadowMapEffect::CreateInputLayout(const VERTEX_BUFFER_DESCRIPTOR& descriptor)
{
	const auto& posVertexItem = descriptor.GetVertexItem(VERTEX_ITEM_ID_POSITION);
	assert(posVertexItem != nullptr);

	typedef std::vector<D3D11_INPUT_ELEMENT_DESC> InputElementArray;

	InputElementArray inputElements;

	{
		D3D11_INPUT_ELEMENT_DESC inputElement = {};
		inputElement.SemanticName			= "POSITION";
		inputElement.SemanticIndex			= 0;
		inputElement.Format					= DXGI_FORMAT_R32G32B32_FLOAT;
		inputElement.AlignedByteOffset		= posVertexItem->offset;
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
