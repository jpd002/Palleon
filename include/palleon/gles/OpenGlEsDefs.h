#pragma once

#ifdef __APPLE__
#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>
#else
#include <GLES2/gl2.h>
#define GL_GLEXT_PROTOTYPES
#include <GLES2/gl2ext.h>
#endif

#ifdef _DEBUG
#define CHECKGLERROR() { assert(glGetError() == GL_NO_ERROR); }
#else
#define CHECKGLERROR()
#endif
