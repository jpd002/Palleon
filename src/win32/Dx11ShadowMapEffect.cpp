#include "athena/win32/Dx11ShadowMapEffect.h"

using namespace Athena;

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

void CDx11ShadowMapEffect::UpdateConstants(const MaterialPtr& material, const CMatrix4& worldMatrix, const CMatrix4& viewMatrix, const CMatrix4& projMatrix,
	const CMatrix4& shadowViewProjMatrix)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource = {};
	HRESULT result = m_deviceContext->Map(m_vertexConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	assert(SUCCEEDED(result));

	auto constantBufferPtr = reinterpret_cast<uint8*>(mappedResource.pData);
	*reinterpret_cast<CMatrix4*>(constantBufferPtr + m_worldMatrixOffset) = worldMatrix;
	*reinterpret_cast<CMatrix4*>(constantBufferPtr + m_viewProjMatrixOffset) = viewMatrix * projMatrix;

	m_deviceContext->Unmap(m_vertexConstantBuffer, 0);
}
