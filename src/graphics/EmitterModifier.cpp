#include "palleon/graphics/EmitterModifier.h"

using namespace Palleon;

CEmitterModifier::CEmitterModifier()
{

}

CEmitterModifier::~CEmitterModifier()
{

}

void CEmitterModifier::Update(float dt)
{
	m_animationController.Update(this, dt);
}

EmitterModifierAnimationController& CEmitterModifier::GetAnimationController()
{
	return m_animationController;
}
