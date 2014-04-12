#ifndef _CONFIGMANAGER_H_
#define _CONFIGMANAGER_H_

#include "Config.h"

namespace Athena
{
	class CConfigManager
	{
	public:

		static void					CreateInstance();
		static void					DestroyInstance();
		static CConfigManager&		GetInstance();

		Framework::CConfig&			GetConfig();

	private:
									CConfigManager();
		virtual						~CConfigManager();

		Framework::CConfig			m_config;

		static CConfigManager*		m_instance;
	};
};

#endif
