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
		long						OnActivate(unsigned int, bool, HWND) override;
		long						OnMouseMove(WPARAM, int, int) override;
		long						OnMouseLeave() override;
		long						OnLeftButtonDown(int, int) override;
		long						OnLeftButtonUp(int, int) override;
		long						OnKeyDown(WPARAM, LPARAM) override;
		long						OnKeyUp(WPARAM, LPARAM) override;

	private:
		void						UpdateApplication();
		static KEY_CODE				TranslateKeyCode(unsigned int);

		uint32						m_screenWidth = 640;
		uint32						m_screenHeight = 480;

		CApplication*				m_application = nullptr;

		int							m_mouseX = 0;
		int							m_mouseY = 0;
		bool						m_mouseDownPending = false;
		bool						m_mouseUpPending = false;
		bool						m_mouseTrackingEnabled = false;
		float						m_frameCounterTime = 0.f;
		unsigned int				m_currentFrameCount = 0;

		LARGE_INTEGER				m_previousTime;
		LARGE_INTEGER				m_counterFreq;
	};
}
