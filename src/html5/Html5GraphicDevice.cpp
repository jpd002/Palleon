#include <iostream>
#include <emscripten.h>
#include "palleon/html5/Html5GraphicDevice.h"
#include "palleon/gles/GlEsVertexBuffer.h"

using namespace Palleon;

//#ifdef _DEBUG
#define CHECKEGLERROR() { assert(eglGetError() == EGL_SUCCESS); }
//#else
//#define CHECKEGLERROR()
//#endif

#define SCREEN_WIDTH	1024
#define SCREEN_HEIGHT	768

CHtml5GraphicDevice::CHtml5GraphicDevice()
: CGlEsGraphicDevice(CVector2(SCREEN_WIDTH, SCREEN_HEIGHT), 1)
{
	static const EGLint configAttribs[] =
	{
		EGL_RED_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_BLUE_SIZE, 8,
		EGL_ALPHA_SIZE, 8,
		EGL_DEPTH_SIZE, 24,
		EGL_STENCIL_SIZE, 8,
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
		EGL_NONE
	};
	
	static const EGLint surfaceAttribs[] =
	{
		EGL_RENDER_BUFFER, EGL_BACK_BUFFER,
		EGL_NONE
	};
	
	static const EGLint contextAttribs[] =
	{
		EGL_CONTEXT_CLIENT_VERSION, 2,
		EGL_NONE
	};

	emscripten_set_canvas_size(SCREEN_WIDTH, SCREEN_HEIGHT);
	
	EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	CHECKEGLERROR();
	assert(display != EGL_NO_DISPLAY);
	
	EGLint majorVersion = 0, minorVersion = 0;
	EGLBoolean result = eglInitialize(display, &majorVersion, &minorVersion);
	CHECKEGLERROR();
	assert(result == EGL_TRUE);
	std::cout << "EGL initialized (major: " << majorVersion << ", minor: " << minorVersion << ")" << std::endl;
	
	EGLConfig config = 0;
	EGLint numConfig = 0;
	result = eglChooseConfig(display, configAttribs, &config, 1, &numConfig);
	CHECKEGLERROR();
	assert(result == EGL_TRUE);
	assert(numConfig == 1);
	std::cout << "Created EGL config." << std::endl;
	
	EGLSurface surface = eglCreateWindowSurface(display, config, 0, surfaceAttribs);
	CHECKEGLERROR();
	assert(surface != EGL_NO_SURFACE);
	std::cout << "Created EGL surface." << std::endl;

	EGLContext context = eglCreateContext(display, config, EGL_NO_CONTEXT, contextAttribs);
	CHECKEGLERROR();
	assert(context != EGL_NO_CONTEXT);
	
	result = eglMakeCurrent(display, surface, surface, context);
	CHECKEGLERROR();
	assert(result == EGL_TRUE);
	
	Initialize();
}

CHtml5GraphicDevice::~CHtml5GraphicDevice()
{

}

void CHtml5GraphicDevice::CreateInstance()
{
	assert(m_instance == nullptr);
	if(m_instance != nullptr) return;
	m_instance = new CHtml5GraphicDevice();
}
