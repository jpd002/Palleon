#pragma once

#include "../AudioClip.h"

namespace Athena
{
	class CIphoneAudioClip : public CAudioClip
	{
	public:
					CIphoneAudioClip();
		virtual		~CIphoneAudioClip();

		void		Play();
	};
}
