#pragma once

#include "../Application.h"

namespace Athena
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
