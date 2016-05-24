#pragma once

#include "../Application.h"
#include "Singleton.h"

namespace Palleon
{
	class CAndroidActivity : public CSingleton<CAndroidActivity>
	{
	public:
		void					Initialize(int, int, float);
		void					Update();
		
		void					NotifyMouseMove(int, int);
		void					NotifyMouseDown();
		void					NotifyMouseUp();
		
	private:
		CApplication*			m_application = nullptr;
	};
}
