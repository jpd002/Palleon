#pragma once

#include <emscripten/html5.h>
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
		static int				MouseMoveCallbackStub(int, const EmscriptenMouseEvent*, void*);
		static int				MouseDownCallbackStub(int, const EmscriptenMouseEvent*, void*);
		static int				MouseUpCallbackStub(int, const EmscriptenMouseEvent*, void*);
		
		void					MainLoopProc();
		int						MouseMoveCallback(int, const EmscriptenMouseEvent*);
		int						MouseDownCallback(int, const EmscriptenMouseEvent*);
		int						MouseUpCallback(int, const EmscriptenMouseEvent*);
		
		CApplication*			m_application = nullptr;
	};
}
