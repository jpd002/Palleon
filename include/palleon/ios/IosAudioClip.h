#pragma once

#include "../AudioClip.h"
#include <AVFoundation/AVFoundation.h>

namespace Palleon
{
	class CIosAudioClip : public CAudioClip
	{
	public:
						CIosAudioClip(const char*);
		virtual			~CIosAudioClip();

		void			Play(bool);
		void			Stop();
		
	private:
		AVAudioPlayer*	m_player;
	};
}
