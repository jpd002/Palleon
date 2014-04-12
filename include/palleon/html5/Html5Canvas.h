#pragma once

#include "../Application.h"

namespace Palleon
{
	class CHtml5Canvas
	{
	public:
								CHtml5Canvas();
		virtual					~CHtml5Canvas();
		
		void					StartMainLoop();
		
	private:
		static void				MainLoopProcStub(void*);
		void					MainLoopProc();
		
		CApplication*			m_application = nullptr;
	};
}
