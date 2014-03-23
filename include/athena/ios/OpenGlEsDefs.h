#pragma once

#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>

#ifdef _DEBUG
#define CHECKGLERROR() { assert(glGetError() == GL_NO_ERROR); }
#else
#define CHECKGLERROR()
#endif
