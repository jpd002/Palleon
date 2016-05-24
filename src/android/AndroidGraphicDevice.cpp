#include <iostream>
#include "palleon/android/AndroidGraphicDevice.h"
#include "palleon/gles/GlEsVertexBuffer.h"

using namespace Palleon;

CAndroidGraphicDevice::CAndroidGraphicDevice(NativeWindowType nativeWindow, int width, int height, float density)
: CGlEsGraphicDevice(CVector2(width, height), density)
, m_window(nativeWindow)
{
	CreateContext();
	SetupContext();
	Initialize();
}

CAndroidGraphicDevice::~CAndroidGraphicDevice()
{

}

void CAndroidGraphicDevice::CreateInstance(NativeWindowType nativeWindow, int width, int height, float density)
{
	assert(m_instance == nullptr);
	if(m_instance != nullptr) return;
	m_instance = new CAndroidGraphicDevice(nativeWindow, width, height, density);
}

SharedGraphicContextPtr CAndroidGraphicDevice::CreateSharedContext()
{
	return SharedGraphicContextPtr();
}

void CAndroidGraphicDevice::PresentBackBuffer()
{
	eglSwapBuffers(m_display, m_surface);
}

void CAndroidGraphicDevice::CreateContext()
{
	static const EGLint configAttribs[] = 
	{
		EGL_SURFACE_TYPE,			EGL_WINDOW_BIT,
		EGL_RENDERABLE_TYPE,		EGL_OPENGL_ES2_BIT,
		EGL_BLUE_SIZE,				8,
		EGL_GREEN_SIZE,				8,
		EGL_RED_SIZE,				8,
		EGL_NONE
	};

	static const EGLint contextAttribs[] =
	{
		EGL_CONTEXT_CLIENT_VERSION,		3,
		EGL_NONE
	};
	
	m_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	eglInitialize(m_display, 0, 0);
	
	EGLint numConfigs = 0;
	eglChooseConfig(m_display, configAttribs, &m_config, 1, &numConfigs);
	assert(numConfigs > 0);
	
	m_context = eglCreateContext(m_display, m_config, NULL, contextAttribs);
	assert(m_context != EGL_NO_CONTEXT);
}

void CAndroidGraphicDevice::SetupContext()
{
	if(m_surface != EGL_NO_SURFACE)
	{
		eglDestroySurface(m_display, m_surface);
		m_surface = EGL_NO_SURFACE;
	}
	
	m_surface = eglCreateWindowSurface(m_display, m_config, m_window, NULL);
	assert(m_surface != EGL_NO_SURFACE);
	
	auto makeCurrentResult = eglMakeCurrent(m_display, m_surface, m_surface, m_context);
	assert(makeCurrentResult != EGL_FALSE);
}
