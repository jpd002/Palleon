#pragma once

#include "AudioClip.h"

namespace Palleon
{
	class CAudioManager
	{
	public:
		virtual							~CAudioManager();

		static CAudioManager&			GetInstance();

		virtual AudioClipPtr			CreateAudioClip(const char*) = 0;

	protected:
		static CAudioManager*			m_instance;
	};
}
