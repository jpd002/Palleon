#include "TargetConditionals.h"
#if !TARGET_IPHONE_SIMULATOR

#include "palleon/ios/MetalUberEffect.h"
#include "palleon/Color.h"
#include "palleon/Material.h"

using namespace Palleon;

CMetalUberEffect::CMetalUberEffect(id<MTLDevice> device, const EFFECTCAPS& effectCaps)
: CMetalEffect(device)
{
	auto librarySource = CMetalUberEffectGenerator::GenerateLibrarySource(effectCaps);
	CreateLibraryAndShaders(librarySource);
	
	OffsetKeeper constantOffset;
	m_worldMatrixOffset = constantOffset.Allocate(sizeof(CMatrix4));
	m_viewProjMatrixOffset = constantOffset.Allocate(sizeof(CMatrix4));
	if(effectCaps.hasShadowMap)
	{
		m_shadowViewProjMatrixOffset = constantOffset.Allocate(sizeof(CMatrix4));
	}
	m_meshColorOffset = constantOffset.Allocate(sizeof(CColor));
	m_constantsSize = constantOffset.currentOffset;
}

CMetalUberEffect::~CMetalUberEffect()
{

}

void CMetalUberEffect::UpdateConstants(uint8* constantBuffer, const METALVIEWPORT_PARAMS& viewportParams, CMaterial* material, const CMatrix4& worldMatrix)
{
	if(m_worldMatrixOffset != -1)
	{
		*reinterpret_cast<CMatrix4*>(constantBuffer + m_worldMatrixOffset) = worldMatrix;
	}
	if(m_viewProjMatrixOffset != -1)
	{
		*reinterpret_cast<CMatrix4*>(constantBuffer + m_viewProjMatrixOffset) = viewportParams.viewMatrix * viewportParams.projMatrix;
	}
	if(m_shadowViewProjMatrixOffset != -1)
	{
		*reinterpret_cast<CMatrix4*>(constantBuffer + m_shadowViewProjMatrixOffset) = viewportParams.shadowViewProjMatrix;
	}
	if(m_meshColorOffset != -1)
	{
		*reinterpret_cast<CColor*>(constantBuffer + m_meshColorOffset) = material->GetColor();
	}
}

unsigned int CMetalUberEffect::GetConstantsSize() const
{
	return m_constantsSize;
}

void CMetalUberEffect::FillPipelinePixelFormats(MTLRenderPipelineDescriptor* pipelineStateDescriptor)
{
	pipelineStateDescriptor.colorAttachments[0].pixelFormat = MTLPixelFormatBGRA8Unorm;
	pipelineStateDescriptor.depthAttachmentPixelFormat = MTLPixelFormatDepth32Float;
}

#endif
