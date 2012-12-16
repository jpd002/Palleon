#pragma once

#include "Mesh.h"

namespace Athena
{
	class CEmitter : public CMesh
	{
	public:
						CEmitter();
		virtual			~CEmitter();

		void			Emit(unsigned int);

		virtual void	Update(float);

	private:
		struct PARTICLE
		{
			float		life;
			float		maxLife;
			CVector2	position;
			CVector2	velocity;
			CVector2	size;
			CColor		color;
		};

		typedef std::vector<PARTICLE> ParticleArray;

		void			UpdateParticles(float);
		void			UpdateVertexBuffer();

		ParticleArray	m_particles;
	};

	typedef std::shared_ptr<CEmitter> EmitterPtr;
}
