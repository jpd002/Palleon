#include <assert.h>
#include "athena/RumbleAnimation.h"
#include "athena/SceneNode.h"
#include "athena/MathOps.h"

using namespace Athena;

CRumbleAnimation::CRumbleAnimation(const CVector3& minPosition, const CVector3& maxPosition, float frequency, float damping, float length)
: m_minPosition(minPosition)
, m_maxPosition(maxPosition)
, m_frequency(frequency)
, m_damping(damping)
, m_length(length)
{

}

CRumbleAnimation::~CRumbleAnimation()
{

}

void CRumbleAnimation::Animate(CSceneNode* target, float t) const
{
	float f = sin(t * m_frequency) * exp(-t / m_damping);
	f = (f + 1.0f) / 2;
	CVector3 result = (f * m_maxPosition) + ((1 - f) * m_minPosition);
	target->SetPosition(result);
}

float CRumbleAnimation::GetLength() const
{
	return m_length;
}
