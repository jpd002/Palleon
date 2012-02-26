#ifndef _OPENGLESDEFS_H_
#define _OPENGLESDEFS_H_

#import <OpenGLES/ES2/gl.h>
#import <OpenGLES/ES2/glext.h>

#ifdef DEBUG
#define CHECKGLERROR() { assert(glGetError() == GL_NO_ERROR); }
#else
#define CHECKGLERROR()
#endif

#endif
