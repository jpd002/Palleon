#include "Dx11WaterEffect.h"

CDx11WaterEffect::CDx11WaterEffect(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
: CDx11Effect(device, deviceContext)
{
	auto vertexShaderText =
		"cbuffer MatrixBuffer"
		"{"
		"	matrix c_worldMatrix;"
		"	matrix c_viewProjMatrix;"
		"	matrix c_reflectViewProjMatrix;"
		"	matrix c_texture2Matrix;"
		"	matrix c_texture3Matrix;"
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
		"	float4 reflectPosition : TEXCOORD0;"
		"	float4 refractPosition : TEXCOORD1;"
		"	float3 eyeVector : TEXCOORD2;"
		"	float2 bumpTexCoord0 : TEXCOORD3;"
		"	float2 bumpTexCoord1 : TEXCOORD4;"
		"};"
		"PixelInputType VertexProgram(VertexInputType input)"
		"{"
		"	PixelInputType output;"
		"	float4 worldPos = mul(c_worldMatrix, float4(input.position, 1));"
		"	output.eyeVector = normalize(c_cameraPosition - worldPos.xyz);"
		"	output.position = mul(c_viewProjMatrix, worldPos);"
		"	output.bumpTexCoord0 = mul(c_texture2Matrix, float4(input.texCoord, 0, 1));"
		"	output.bumpTexCoord1 = mul(c_texture3Matrix, float4(input.texCoord, 0, 1));"
		"	output.reflectPosition = mul(c_reflectViewProjMatrix, worldPos);"
		"	output.refractPosition = output.position;"
		"	return output;"
		"}";
	auto pixelShaderText = 
		"struct PixelInputType"
		"{"
		"	float4 position : SV_POSITION;"
		"	float4 reflectPosition : TEXCOORD0;"
		"	float4 refractPosition : TEXCOORD1;"
		"	float3 eyeVector : TEXCOORD2;"
		"	float2 bumpTexCoord0 : TEXCOORD3;"
		"	float2 bumpTexCoord1 : TEXCOORD4;"
		"};"
		"Texture2D c_reflectTexture		: register(t0);"
		"Texture2D c_refractTexture		: register(t1);"
		"Texture2D c_bumpTexture0		: register(t2);"
		"Texture2D c_bumpTexture1		: register(t3);"
		"SamplerState c_reflectSampler	: register(s0);"
		"SamplerState c_refractSampler	: register(s1);"
		"SamplerState c_bumpSampler0	: register(s2);"
		"SamplerState c_bumpSampler1	: register(s3);"
		"float4 PixelProgram(PixelInputType input) : SV_TARGET"
		"{"
		"	float waveHeight = 0.03f;"
		"	float4 dullColor = float4(0.2f, 0.1f, 0.0f, 1.0f);"
		"	float3 lightDir = normalize(float3(-1.0f, 1.0f, 1.0f));"
		"	float4 bump0 = c_bumpTexture0.Sample(c_bumpSampler0, input.bumpTexCoord0);"
		"	float4 bump1 = c_bumpTexture1.Sample(c_bumpSampler1, input.bumpTexCoord1);"
		"	float3 normal = ((((bump0.rbg + bump1.rbg) * 0.5f) - 0.5f) * 2.0f);"
		"	normal = normalize(normal);"
		"	float2 perturbation = waveHeight * normal.xz;"
		"	float2 reflectTexCoord;"
		"	float2 refractTexCoord;"
		"	reflectTexCoord.x = ( input.reflectPosition.x / input.reflectPosition.w) / 2.0f + 0.5f;"
		"	reflectTexCoord.y = (-input.reflectPosition.y / input.reflectPosition.w) / 2.0f + 0.5f;"
		"	refractTexCoord.x = ( input.refractPosition.x / input.refractPosition.w) / 2.0f + 0.5f;"
		"	refractTexCoord.y = (-input.refractPosition.y / input.refractPosition.w) / 2.0f + 0.5f;"
		"	reflectTexCoord += perturbation;"
		"	refractTexCoord += perturbation;"
		"	float4 reflectColor = c_reflectTexture.Sample(c_reflectSampler, reflectTexCoord);"
		"	float4 refractColor = c_refractTexture.Sample(c_refractSampler, refractTexCoord);"
		"	float fresnelTerm = saturate(dot(input.eyeVector, normal) * 1.3f);"
		"	float3 halfVector = normalize(lightDir + input.eyeVector);"
		"	float specularFactor = max(dot(normal, halfVector), 0);"
		"	float specular = pow(specularFactor, 255);"
		"	float diffuse = max(dot(normal, lightDir), 0);"
		"	float4 combinedColor = lerp(reflectColor, refractColor, fresnelTerm);"
		"	return lerp(dullColor, combinedColor, 0.2f) + float4(diffuse, diffuse, diffuse, diffuse) * 0.02f + float4(specular, specular, specular, 0);"
		"}";

	CompileVertexShader(vertexShaderText);
	CompilePixelShader(pixelShaderText);

	{
		OffsetKeeper constantOffset;

		m_worldMatrixOffset				= constantOffset.Allocate(0x40);
		m_viewProjMatrixOffset			= constantOffset.Allocate(0x40);
		m_reflectViewProjMatrixOffset	= constantOffset.Allocate(0x40);
		m_texture2MatrixOffset			= constantOffset.Allocate(0x40);
		m_texture3MatrixOffset			= constantOffset.Allocate(0x40);
		m_cameraPositionOffset			= constantOffset.Allocate(0x10);

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

	auto clipPlane = CVector4(0, 1, 0, 0);

	auto reflectMatrix = CMatrix4::MakeReflect(clipPlane.x, clipPlane.y, clipPlane.z, clipPlane.w);

	auto reflectViewMatrix = reflectMatrix * viewportParams.viewMatrix;

	auto inverseView = viewportParams.viewMatrix.Inverse();
	auto cameraPosition = CVector4(inverseView(3, 0), inverseView(3, 1), inverseView(3, 2), 0);

	auto constantBufferPtr = reinterpret_cast<uint8*>(mappedResource.pData);
	*reinterpret_cast<CMatrix4*>(constantBufferPtr + m_worldMatrixOffset) = worldMatrix;
	*reinterpret_cast<CMatrix4*>(constantBufferPtr + m_viewProjMatrixOffset) = viewportParams.viewMatrix * viewportParams.projMatrix;
	*reinterpret_cast<CMatrix4*>(constantBufferPtr + m_reflectViewProjMatrixOffset) = reflectViewMatrix * viewportParams.projMatrix;
	*reinterpret_cast<CMatrix4*>(constantBufferPtr + m_texture2MatrixOffset) = material->GetTextureMatrix(2);
	*reinterpret_cast<CMatrix4*>(constantBufferPtr + m_texture3MatrixOffset) = material->GetTextureMatrix(3);
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
