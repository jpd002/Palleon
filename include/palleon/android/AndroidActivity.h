#pragma once

#include "../Application.h"

namespace Palleon
{
	class AndroidActivity
	{
	public:
		void					Initialize(int, int);
		void					Update();
		
	private:
		CApplication*			m_application = nullptr;
		bool					m_initialized = false;
	};
}
