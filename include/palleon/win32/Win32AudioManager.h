#pragma once

#include "../AudioManager.h"

namespace Palleon
{
	class CWin32AudioManager : public CAudioManager
	{
	public:
		static void				CreateInstance();
		static void				DestroyInstance();

		virtual AudioClipPtr	CreateAudioClip(const char*);

	private:
								CWin32AudioManager();
		virtual					~CWin32AudioManager();
	};
}
