#include "Dx11WaterEffect.h"

CDx11WaterEffect::CDx11WaterEffect(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
: CDx11Effect(device, deviceContext)
{
	auto vertexShaderText =
		"cbuffer MatrixBuffer"
		"{"
		"	matrix c_worldMatrix;"
		"	matrix c_viewProjMatrix;"
		"	matrix c_texture1Matrix;"
		"	float3 c_cameraPosition;"
		"};"
		"struct VertexInputType"
		"{"
		"	float3 position : POSITION;"
		"	float2 texCoord : TEXCOORD;"
		"};"
		"struct PixelInputType"
		"{"
		"	float4 position : SV_POSITION;"
		"	float2 texCoord1 : TEXCOORD0;"
		"	float4 refractPosition : TEXCOORD1;"
		"	float3 eyeVector : TEXCOORD2;"
		"};"
		"PixelInputType VertexProgram(VertexInputType input)"
		"{"
		"	PixelInputType output;"
		"	output.position = mul(c_worldMatrix, float4(input.position, 1));"
		"	output.eyeVector = normalize(c_cameraPosition - output.position.xyz);"
		"	output.position = mul(c_viewProjMatrix, output.position);"
		"	output.texCoord1 = mul(c_texture1Matrix, float4(input.texCoord, 0, 1));"
		"	output.refractPosition = output.position;"
		"	return output;"
		"}";
	auto pixelShaderText = 
		"struct PixelInputType"
		"{"
		"	float4 position : SV_POSITION;"
		"	float2 texCoord1 : TEXCOORD0;"
		"	float4 refractPosition : TEXCOORD1;"
		"	float3 eyeVector : TEXCOORD2;"
		"};"
		"Texture2D c_reflectionTexture : register(t0);"
		"Texture2D c_bumpTexture : register(t1);"
		"SamplerState c_reflectionSampler : register(s0);"
		"SamplerState c_bumpSampler : register(s1);"
		"float4 PixelProgram(PixelInputType input) : SV_TARGET"
		"{"
		"	float4 bump = c_bumpTexture.Sample(c_bumpSampler, input.texCoord1);"
		"	float waveHeight = 0.2f;"
		"	float2 perturbation = waveHeight * ((bump.rg - 0.5f) * 2.0f);"
		"	float2 refractTexCoord;"
		"	refractTexCoord.x = ( input.refractPosition.x / input.refractPosition.w) / 2.0f + 0.5f;"
		"	refractTexCoord.y = (-input.refractPosition.y / input.refractPosition.w) / 2.0f + 0.5f;"
		"	refractTexCoord += perturbation;"
		"	float4 refractColor = c_reflectionTexture.Sample(c_reflectionSampler, refractTexCoord);"
		"	float3 normalVector = float3(0, 1, 0);"
		"	float fresnelTerm = dot(input.eyeVector, normalVector);"
		"	float4 combinedColor = lerp(float4(0, 0, 0, 0), refractColor, fresnelTerm);"
		"	float4 dullColor = float4(0.3f, 0.3f, 0.5f, 1.0f);"
		"	return lerp(combinedColor, dullColor, 0.2f);"
		"}";

	CompileVertexShader(vertexShaderText);
	CompilePixelShader(pixelShaderText);

	{
		OffsetKeeper constantOffset;

		m_worldMatrixOffset		= constantOffset.Allocate(0x40);
		m_viewProjMatrixOffset	= constantOffset.Allocate(0x40);
		m_texture1MatrixOffset	= constantOffset.Allocate(0x40);
		m_cameraPositionOffset	= constantOffset.Allocate(0x10);

		CreateVertexConstantBuffer(constantOffset.currentOffset);
	}
}

CDx11WaterEffect::~CDx11WaterEffect()
{

}

void CDx11WaterEffect::UpdateConstants(const Palleon::DX11VIEWPORT_PARAMS& viewportParams, Palleon::CMaterial* material, const CMatrix4& worldMatrix)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource = {};
	HRESULT result = m_deviceContext->Map(m_vertexConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	assert(SUCCEEDED(result));

	auto inverseView = viewportParams.viewMatrix.Inverse();
	auto cameraPosition = CVector4(inverseView(3, 0), inverseView(3, 1), inverseView(3, 2), 0);

	auto constantBufferPtr = reinterpret_cast<uint8*>(mappedResource.pData);
	*reinterpret_cast<CMatrix4*>(constantBufferPtr + m_worldMatrixOffset) = worldMatrix;
	*reinterpret_cast<CMatrix4*>(constantBufferPtr + m_viewProjMatrixOffset) = viewportParams.viewMatrix * viewportParams.projMatrix;
	*reinterpret_cast<CMatrix4*>(constantBufferPtr + m_texture1MatrixOffset) = material->GetTextureMatrix(1);
	*reinterpret_cast<CVector4*>(constantBufferPtr + m_cameraPositionOffset) = cameraPosition;

	m_deviceContext->Unmap(m_vertexConstantBuffer, 0);
}

Palleon::CDx11Effect::D3D11InputLayoutPtr CDx11WaterEffect::CreateInputLayout(const Palleon::VERTEX_BUFFER_DESCRIPTOR& descriptor)
{
	const auto& posVertexItem = descriptor.GetVertexItem(Palleon::VERTEX_ITEM_ID_POSITION);
	const auto& texCoordItem = descriptor.GetVertexItem(Palleon::VERTEX_ITEM_ID_UV0);
	assert(posVertexItem != nullptr);
	assert(texCoordItem != nullptr);

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

	{
		D3D11_INPUT_ELEMENT_DESC inputElement = {};
		inputElement.SemanticName			= "TEXCOORD";
		inputElement.SemanticIndex			= 0;
		inputElement.Format					= DXGI_FORMAT_R32G32_FLOAT;
		inputElement.AlignedByteOffset		= texCoordItem->offset;
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
