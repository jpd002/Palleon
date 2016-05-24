#pragma once

#include <android/native_window.h>
#include "Types.h"
#include "../Application.h"
#include "Singleton.h"

namespace Palleon
{
	class CAndroidActivity : public CSingleton<CAndroidActivity>
	{
	public:
		void					Initialize(ANativeWindow*, int, int, float);
		void					Update(uint64);
		
		void					NotifyMouseMove(int, int);
		void					NotifyMouseDown();
		void					NotifyMouseUp();
		
	private:
		CApplication*			m_application = nullptr;
		uint64					m_lastFrameTime = 0;
	};
}
