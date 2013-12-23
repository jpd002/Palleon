#include "athena/win32/Dx11UberEffect.h"

using namespace Athena;

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

	CreateConstantBuffer(constantOffset.currentOffset);
}

CDx11UberEffect::~CDx11UberEffect()
{

}

void CDx11UberEffect::UpdateConstants(const MaterialPtr& material, const CMatrix4& worldMatrix, const CMatrix4& viewProjMatrix, const CMatrix4& shadowViewProjMatrix)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource = {};
	HRESULT result = m_deviceContext->Map(m_constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	assert(SUCCEEDED(result));

	auto constantBufferPtr = reinterpret_cast<uint8*>(mappedResource.pData);
	if(m_meshColorOffset != -1)
	{
		*reinterpret_cast<CColor*>(constantBufferPtr + m_meshColorOffset) = material->GetColor();
	}
	*reinterpret_cast<CMatrix4*>(constantBufferPtr + m_worldMatrixOffset) = worldMatrix;
	*reinterpret_cast<CMatrix4*>(constantBufferPtr + m_viewProjMatrixOffset) = viewProjMatrix;
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

	m_deviceContext->Unmap(m_constantBuffer, 0);
}
