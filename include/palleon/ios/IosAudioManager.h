#pragma once

#include "../AudioManager.h"

namespace Palleon
{
	class CIosAudioManager : public CAudioManager
	{
	public:
		static void				CreateInstance();
		static void				DestroyInstance();

		virtual AudioClipPtr	CreateAudioClip(const char*);

	private:
								CIosAudioManager();
		virtual					~CIosAudioManager();
	};
}
