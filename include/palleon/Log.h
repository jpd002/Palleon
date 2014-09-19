#pragma once

namespace Palleon
{
	class CLog
	{
	public:
		virtual						~CLog() {}

		static CLog&				GetInstance();

		virtual void				Print(const char*, ...) = 0;

	protected:
		static CLog*				m_instance;
	};
}
