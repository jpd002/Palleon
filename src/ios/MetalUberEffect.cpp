#include "palleon/ios/MetalUberEffect.h"
#include "palleon/Color.h"
#include "palleon/Material.h"

using namespace Palleon;

struct CONSTANTS
{
	CMatrix4	modelViewProjMatrix;
	CColor		meshColor;
};

CMetalUberEffect::CMetalUberEffect(id<MTLDevice> device, const EFFECTCAPS& effectCaps)
: CMetalEffect(device)
{
	auto librarySource = CMetalUberEffectGenerator::GenerateLibrarySource(effectCaps);
	CreateLibraryAndShaders(librarySource);
}

CMetalUberEffect::~CMetalUberEffect()
{

}

void CMetalUberEffect::UpdateConstants(void* constantBuffer, const METALVIEWPORT_PARAMS& viewportParams, CMaterial* material, const CMatrix4& worldMatrix)
{
	CONSTANTS* constants = reinterpret_cast<CONSTANTS*>(constantBuffer);
	constants->modelViewProjMatrix = worldMatrix * viewportParams.viewMatrix * viewportParams.projMatrix;
	constants->meshColor = material->GetColor();
}

unsigned int CMetalUberEffect::GetConstantsSize() const
{
	return sizeof(CONSTANTS);
}
