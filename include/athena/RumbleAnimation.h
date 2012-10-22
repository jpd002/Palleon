#pragma once

#include "IAnimation.h"
#include "Vector3.h"

namespace Athena
{
	class CRumbleAnimation : public IAnimation
	{
	public:
							CRumbleAnimation(const CVector3& minPosition, const CVector3& maxPosition, float frequency, float damping, float length);
		virtual				~CRumbleAnimation();

		void				Animate(CSceneNode*, float) const;
		float				GetLength() const;

	private:
		CVector3			m_minPosition;
		CVector3			m_maxPosition;
		float				m_frequency;
		float				m_damping;
		float				m_length;
	};
};
