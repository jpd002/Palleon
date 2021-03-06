#pragma once

#include <memory>
#include "palleon/graphics/Mesh.h"
#include "palleon/resources/EmitterDescriptor.h"

namespace Palleon
{
	class CEmitterModifier;
	typedef std::shared_ptr<CEmitterModifier> EmitterModifierPtr;

	class CEmitter : public CMesh
	{
	public:
		struct PARTICLE
		{
			float		life;
			float		maxLife;
			CVector2	position;
			CVector2	velocity;
			CVector2	size;
			CColor		color;
		};

						CEmitter();
		virtual			~CEmitter();

		void			SetDescriptor(const CEmitterDescriptor*);

		void			AddModifier(const EmitterModifierPtr&);

		void			Emit(unsigned int);

		virtual void	Update(float);

	private:
		typedef std::vector<PARTICLE> ParticleArray;
		typedef std::vector<EmitterModifierPtr> EmitterModifierArray;

		void						UpdateParticles(float);
		void						UpdateVertexBuffer();

		const CEmitterDescriptor*	m_descriptor;
		ParticleArray				m_particles;
		EmitterModifierArray		m_modifiers;
	};

	typedef std::shared_ptr<CEmitter> EmitterPtr;
}
