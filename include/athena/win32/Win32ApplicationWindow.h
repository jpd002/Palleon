#pragma once

#include "Types.h"
#include "win32/Window.h"
#include "../Application.h"

namespace Athena
{
	class CWin32ApplicationWindow : public Framework::Win32::CWindow
	{
	public:
									CWin32ApplicationWindow();
		virtual						~CWin32ApplicationWindow();

		void						Loop();

	protected:
		long						OnMouseMove(WPARAM, int, int);
		long						OnLeftButtonDown(int, int);
		long						OnLeftButtonUp(int, int);

	private:
		void						UpdateApplication();
		void						CreateDevice();

		uint32						m_screenWidth;
		uint32						m_screenHeight;

		CApplication*				m_application;

		int							m_mouseX;
		int							m_mouseY;
		bool						m_mouseDownPending;
		bool						m_mouseUpPending;
		float						m_frameCounterTime;
		unsigned int				m_currentFrameCount;

		LARGE_INTEGER				m_previousTime;
		LARGE_INTEGER				m_counterFreq;
	};
}
