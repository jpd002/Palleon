#include "palleon/win32/Dx11UberEffect.h"

using namespace Palleon;

CDx11UberEffect::CDx11UberEffect(ID3D11Device* device, ID3D11DeviceContext* deviceContext, const EFFECTCAPS& effectCaps)
: CDx11Effect(device, deviceContext)
, m_effectCaps(effectCaps)
{
	memset(&m_diffuseTextureMatrixOffset, -1, sizeof(m_diffuseTextureMatrixOffset));

	auto vertexShaderText = CDx11UberEffectGenerator::GenerateVertexShader(effectCaps);
	auto pixelShaderText = CDx11UberEffectGenerator::GeneratePixelShader(effectCaps);

	CompileVertexShader(vertexShaderText);
	CompilePixelShader(pixelShaderText);

	OffsetKeeper constantOffset;

	m_meshColorOffset				= constantOffset.Allocate(0x10);
	m_worldMatrixOffset				= constantOffset.Allocate(0x40);
	m_viewProjMatrixOffset			= constantOffset.Allocate(0x40);
	
	if(effectCaps.hasShadowMap)		m_shadowViewProjMatrixOffset	= constantOffset.Allocate(0x40);
	if(effectCaps.hasDiffuseMap0)	m_diffuseTextureMatrixOffset[0] = constantOffset.Allocate(0x40);
	if(effectCaps.hasDiffuseMap1)	m_diffuseTextureMatrixOffset[1] = constantOffset.Allocate(0x40);
	if(effectCaps.hasDiffuseMap2)	m_diffuseTextureMatrixOffset[2] = constantOffset.Allocate(0x40);
	if(effectCaps.hasDiffuseMap3)	m_diffuseTextureMatrixOffset[3] = constantOffset.Allocate(0x40);
	if(effectCaps.hasDiffuseMap4)	m_diffuseTextureMatrixOffset[4] = constantOffset.Allocate(0x40);

	CreateVertexConstantBuffer(constantOffset.currentOffset);
}

CDx11UberEffect::~CDx11UberEffect()
{

}

void CDx11UberEffect::UpdateConstants(const MaterialPtr& material, const CMatrix4& worldMatrix, const CMatrix4& viewMatrix, const CMatrix4& projMatrix,
	const CMatrix4& shadowViewProjMatrix)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource = {};
	HRESULT result = m_deviceContext->Map(m_vertexConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	assert(SUCCEEDED(result));

	auto constantBufferPtr = reinterpret_cast<uint8*>(mappedResource.pData);
	if(m_meshColorOffset != -1)
	{
		*reinterpret_cast<CColor*>(constantBufferPtr + m_meshColorOffset) = material->GetColor();
	}
	*reinterpret_cast<CMatrix4*>(constantBufferPtr + m_worldMatrixOffset) = worldMatrix;
	*reinterpret_cast<CMatrix4*>(constantBufferPtr + m_viewProjMatrixOffset) = viewMatrix * projMatrix;
	if(m_shadowViewProjMatrixOffset != -1)
	{
		*reinterpret_cast<CMatrix4*>(constantBufferPtr + m_shadowViewProjMatrixOffset) = shadowViewProjMatrix;
	}

	for(unsigned int i = 0; i < CDx11UberEffectGenerator::MAX_DIFFUSE_SLOTS; i++)
	{
		if(m_diffuseTextureMatrixOffset[i] != -1)
		{
			const auto& textureMatrix = material->GetTextureMatrix(i);
			*reinterpret_cast<CMatrix4*>(constantBufferPtr + m_diffuseTextureMatrixOffset[i]) = textureMatrix;
		}
	}

	m_deviceContext->Unmap(m_vertexConstantBuffer, 0);
}

CDx11Effect::D3D11InputLayoutPtr CDx11UberEffect::CreateInputLayout(const VERTEX_BUFFER_DESCRIPTOR& descriptor)
{
	typedef std::vector<D3D11_INPUT_ELEMENT_DESC> InputElementArray;

	InputElementArray inputElements;

	if(const auto& item = descriptor.GetVertexItem(VERTEX_ITEM_ID_POSITION))
	{
		D3D11_INPUT_ELEMENT_DESC inputElement = {};
		inputElement.SemanticName			= "POSITION";
		inputElement.SemanticIndex			= 0;
		inputElement.Format					= DXGI_FORMAT_R32G32B32_FLOAT;
		inputElement.AlignedByteOffset		= item->offset;
		inputElement.InputSlotClass			= D3D11_INPUT_PER_VERTEX_DATA;
		inputElement.InstanceDataStepRate	= 0;
		inputElements.push_back(inputElement);
	}

	if(const auto& item = descriptor.GetVertexItem(VERTEX_ITEM_ID_NORMAL))
	{
		D3D11_INPUT_ELEMENT_DESC inputElement = {};
		inputElement.SemanticName			= "NORMAL";
		inputElement.SemanticIndex			= 0;
		inputElement.Format					= DXGI_FORMAT_R32G32B32_FLOAT;
		inputElement.AlignedByteOffset		= item->offset;
		inputElement.InputSlotClass			= D3D11_INPUT_PER_VERTEX_DATA;
		inputElement.InstanceDataStepRate	= 0;
		inputElements.push_back(inputElement);
	}

	if(const auto& item = descriptor.GetVertexItem(VERTEX_ITEM_ID_UV0))
	{
		D3D11_INPUT_ELEMENT_DESC inputElement = {};
		inputElement.SemanticName			= "TEXCOORD";
		inputElement.SemanticIndex			= 0;
		inputElement.Format					= DXGI_FORMAT_R32G32_FLOAT;
		inputElement.AlignedByteOffset		= item->offset;
		inputElement.InputSlotClass			= D3D11_INPUT_PER_VERTEX_DATA;
		inputElement.InstanceDataStepRate	= 0;
		inputElements.push_back(inputElement);
	}

	if(const auto& item = descriptor.GetVertexItem(VERTEX_ITEM_ID_UV1))
	{
		D3D11_INPUT_ELEMENT_DESC inputElement = {};
		inputElement.SemanticName			= "TEXCOORD";
		inputElement.SemanticIndex			= 1;
		inputElement.Format					= DXGI_FORMAT_R32G32_FLOAT;
		inputElement.AlignedByteOffset		= item->offset;
		inputElement.InputSlotClass			= D3D11_INPUT_PER_VERTEX_DATA;
		inputElement.InstanceDataStepRate	= 0;
		inputElements.push_back(inputElement);
	}

	if(const auto& item = descriptor.GetVertexItem(VERTEX_ITEM_ID_COLOR))
	{
		D3D11_INPUT_ELEMENT_DESC inputElement = {};
		inputElement.SemanticName			= "COLOR";
		inputElement.SemanticIndex			= 0;
		inputElement.Format					= DXGI_FORMAT_R8G8B8A8_UNORM;
		inputElement.AlignedByteOffset		= item->offset;
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
