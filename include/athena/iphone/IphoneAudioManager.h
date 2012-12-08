#pragma once

#include "../AudioManager.h"

namespace Athena
{
	class CIphoneAudioManager : public CAudioManager
	{
	public:
		static void				CreateInstance();
		static void				DestroyInstance();

		virtual AudioClipPtr	CreateAudioClip(const char*);

	private:
								CIphoneAudioManager();
		virtual					~CIphoneAudioManager();
	};
}
