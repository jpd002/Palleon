#pragma once

#include "Emitter.h"
#include "AnimationController.h"

namespace Palleon
{
	class CEmitterModifier;

	typedef Palleon::CAnimationController<CEmitterModifier> EmitterModifierAnimationController;
	typedef EmitterModifierAnimationController::AnimationPtr EmitterModifierAnimationPtr;

	class CEmitterModifier
	{
	public:
												CEmitterModifier();
		virtual									~CEmitterModifier();

		virtual void							Update(float);
		virtual void							Modify(float, CEmitter::PARTICLE&) = 0;

		EmitterModifierAnimationController&		GetAnimationController();

	private:
		EmitterModifierAnimationController		m_animationController;
	};
}
