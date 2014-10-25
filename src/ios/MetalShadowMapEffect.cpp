#include "TargetConditionals.h"
#if !TARGET_IPHONE_SIMULATOR

#include "palleon/ios/MetalShadowMapEffect.h"

using namespace Palleon;

struct CONSTANTS
{
	CMatrix4	worldViewProjMatrix;
};

CMetalShadowMapEffect::CMetalShadowMapEffect(id<MTLDevice> device, const EFFECTCAPS& effectCaps)
: CMetalEffect(device)

{
	auto librarySource = CMetalShadowMapEffectGenerator::GenerateLibrarySource(effectCaps);
	CreateLibraryAndShaders(librarySource);
}

CMetalShadowMapEffect::~CMetalShadowMapEffect()
{

}

void CMetalShadowMapEffect::UpdateConstants(const VIEWPORT_PARAMS& viewportParams, CMaterial* material, const CMatrix4& worldMatrix)
{
	CONSTANTS* constants = reinterpret_cast<CONSTANTS*>(m_constantBuffer);
	constants->worldViewProjMatrix = worldMatrix * viewportParams.viewMatrix * viewportParams.projMatrix;
}

unsigned int CMetalShadowMapEffect::GetConstantsSize() const
{
	return sizeof(CONSTANTS);
}

void CMetalShadowMapEffect::FillPipelinePixelFormats(MTLRenderPipelineDescriptor* pipelineStateDescriptor)
{
	pipelineStateDescriptor.colorAttachments[0].pixelFormat = MTLPixelFormatInvalid;
	pipelineStateDescriptor.depthAttachmentPixelFormat = MTLPixelFormatDepth32Float;
}

#endif
