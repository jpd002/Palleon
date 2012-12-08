#pragma once

#include "../AudioClip.h"

namespace Athena
{
	class CWin32AudioClip : public CAudioClip
	{
	public:
					CWin32AudioClip();
		virtual		~CWin32AudioClip();

		void		Play();
	};
}
