#pragma once

#include <android/native_window.h>
#include "../gles/GlEsGraphicDevice.h"

namespace Palleon
{
	class CAndroidGlEsGraphicDevice : public CGlEsGraphicDevice
	{
	public:
		static void						CreateInstance(NativeWindowType, int, int, float);
		
		void							PresentBackBuffer();
		
		virtual SharedGraphicContextPtr	CreateSharedContext() override;
		
	private:
										CAndroidGlEsGraphicDevice(NativeWindowType, int, int, float);
		virtual							~CAndroidGlEsGraphicDevice();
		
		void							CreateContext();
		void							SetupContext();
		
		NativeWindowType				m_window = nullptr;
		EGLConfig						m_config = 0;
		EGLDisplay						m_display = EGL_NO_DISPLAY;
		EGLContext						m_context = EGL_NO_CONTEXT;
		EGLSurface						m_surface = EGL_NO_SURFACE;
	};
}
