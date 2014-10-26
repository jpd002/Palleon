#pragma once

#include "../PlatformDef.h"

#if defined(PALLEON_IOS)
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>
#elif defined(PALLEON_ANDROID)
#include <GLES3/gl3.h>
#define GL_GLEXT_PROTOTYPES
#include <GLES3/gl3ext.h>
#else	//Emscripten/WebGL
#include <GLES2/gl2.h>
#define GL_GLEXT_PROTOTYPES
#include <GLES2/gl2ext.h>
#endif

#ifdef _DEBUG
#define CHECKGLERROR() { assert(glGetError() == GL_NO_ERROR); }
#else
#define CHECKGLERROR()
#endif
