#pragma once

#include "../Log.h"

namespace Palleon
{
	class CAndroidLog : public CLog
	{
	public:
		static void				CreateInstance();
		
		virtual void			Print(const char*, ...) override;
		
	private:
	
	};
}
