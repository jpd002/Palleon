#ifndef _DX9APPLICATIONWINDOW_H_
#define _DX9APPLICATIONWINDOW_H_

#include "Types.h"
#include "win32/Window.h"
#include "../Application.h"
#include <d3d9.h>
#include <d3dx9.h>

namespace Athena
{
	class CDx9ApplicationWindow : public Framework::Win32::CWindow
	{
	public:
									CDx9ApplicationWindow();
		virtual						~CDx9ApplicationWindow();

		void						Loop();

	protected:
		long						OnMouseMove(WPARAM, int, int);
		long						OnLeftButtonDown(int, int);
		long						OnLeftButtonUp(int, int);

	private:
		void						UpdateApplication();
		void						CreateDevice();

		IDirect3D9*					m_d3d;
		IDirect3DDevice9*			m_device;

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

#endif
