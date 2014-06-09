#include "palleon/ios/MetalUberEffect.h"

using namespace Palleon;

CMetalUberEffect::CMetalUberEffect(id<MTLDevice> device, const EFFECTCAPS& effectCaps)
: CMetalEffect(device)
{
	auto librarySource = CMetalUberEffectGenerator::GenerateLibrarySource(effectCaps);
	CreateLibraryAndShaders(librarySource);
}

CMetalUberEffect::~CMetalUberEffect()
{

}

void CMetalUberEffect::UpdateConstants(void* constantBuffer, const METALVIEWPORT_PARAMS& viewportParams, const CMatrix4& worldMatrix)
{
	auto modelViewProjMatrix = worldMatrix * viewportParams.viewMatrix * viewportParams.projMatrix;
	memcpy(constantBuffer, &modelViewProjMatrix, sizeof(CMatrix4));
}

unsigned int CMetalUberEffect::GetConstantsSize() const
{
	return sizeof(CMatrix4);
}
