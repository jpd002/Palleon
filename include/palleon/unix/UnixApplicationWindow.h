#pragma once

#include <xcb/xcb.h>
#include "../Application.h"

namespace Palleon
{
	class CUnixApplicationWindow
	{
	public:
		CUnixApplicationWindow();
		~CUnixApplicationWindow();

		void Loop();

	private:
		xcb_connection_t* m_connection = nullptr;
		uint32_t          m_window = 0;

		CApplication* m_application = nullptr;
	};
}
