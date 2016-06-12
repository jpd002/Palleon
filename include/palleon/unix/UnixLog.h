#pragma once

#include "../Log.h"

namespace Palleon
{
	class CUnixLog : public CLog
	{
	public:
		static void    CreateInstance();

		void Print(const char*, ...) override;
	};
}
