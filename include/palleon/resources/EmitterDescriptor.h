#pragma once

#include "../Vector2.h"
#include "Resource.h"
#include <algorithm>

namespace Palleon
{
	enum EMITTER_SOURCE_TYPE
	{
		EMITTER_SOURCE_POINT,
		EMITTER_SOURCE_LINE
	};

	typedef std::pair<float, float> FloatRange;
	typedef std::pair<CVector2, CVector2> Vector2Range;

	class CEmitterDescriptor : public CResource
	{
	public:
								CEmitterDescriptor();
		virtual					~CEmitterDescriptor();

		void					Load(const char*) override;

		EMITTER_SOURCE_TYPE		GetSourceType() const;

		CVector2				GetLineSourceStart() const;
		CVector2				GetLineSourceEnd() const;

		FloatRange				GetInitialVelocityRange() const;
		FloatRange				GetInitialLifeRange() const;
		Vector2Range			GetInitialSizeRange() const;

	private:
		EMITTER_SOURCE_TYPE		m_sourceType;
		CVector2				m_lineStart;
		CVector2				m_lineEnd;
		FloatRange				m_initialVelocityRange;
		FloatRange				m_initialLifeRange;
		Vector2Range			m_initialSizeRange;
	};
}
