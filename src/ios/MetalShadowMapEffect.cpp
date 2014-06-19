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

void CMetalShadowMapEffect::UpdateConstants(uint8* constantBuffer, const METALVIEWPORT_PARAMS& viewportParams, CMaterial* material, const CMatrix4& worldMatrix)
{
	CONSTANTS* constants = reinterpret_cast<CONSTANTS*>(constantBuffer);
	constants->worldViewProjMatrix = worldMatrix * viewportParams.viewMatrix * viewportParams.projMatrix;
}

unsigned int CMetalShadowMapEffect::GetConstantsSize() const
{
	return sizeof(CONSTANTS);
}

void CMetalShadowMapEffect::FillPipelinePixelFormats(MTLRenderPipelineDescriptor* pipelineStateDescriptor)
{
	MTLRenderPipelineAttachmentDescriptor* colorDescriptor = [MTLRenderPipelineAttachmentDescriptor new];
	colorDescriptor.pixelFormat = MTLPixelFormatInvalid;
	[pipelineStateDescriptor.colorAttachments setObject: colorDescriptor atIndexedSubscript: 0];
	pipelineStateDescriptor.depthAttachmentPixelFormat = MTLPixelFormatDepth32Float;
}

