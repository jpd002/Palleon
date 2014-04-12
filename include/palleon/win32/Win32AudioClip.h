#pragma once

#include "../AudioClip.h"

namespace Palleon
{
	class CWin32AudioClip : public CAudioClip
	{
	public:
					CWin32AudioClip();
		virtual		~CWin32AudioClip();

		void		Play(bool);
		void		Stop();
	};
}
