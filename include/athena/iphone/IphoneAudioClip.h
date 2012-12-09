#pragma once

#include "../AudioClip.h"
#include <AVFoundation/AVFoundation.h>

namespace Athena
{
	class CIphoneAudioClip : public CAudioClip
	{
	public:
						CIphoneAudioClip(const char*);
		virtual			~CIphoneAudioClip();

		void			Play(bool);
		
	private:
		AVAudioPlayer*	m_player;
	};
}
