#import <UIKit/UIKit.h>
#include "OpenGlEsDefs.h"

// This class wraps the CAEAGLLayer from CoreAnimation into a convenient UIView subclass.
// The view content is basically an EAGL surface you render your OpenGL scene into.
// Note that setting the view non-opaque will only work if the EAGL surface has an alpha channel.
@interface EAGLView : UIView
{
@private
	EAGLContext *context;
	
	// The pixel dimensions of the CAEAGLLayer.
	GLint framebufferWidth;
	GLint framebufferHeight;
	
	// The OpenGL ES names for the framebuffer and renderbuffer used to render to this view.
	GLuint defaultFramebuffer, colorRenderbuffer, depthRenderbuffer;
	
	GLuint msFramebuffer, msColorRenderbuffer, msDepthRenderbuffer;
	
	BOOL hasRetinaDisplay;
}

@property (nonatomic, retain) EAGLContext* context;

-(BOOL)presentFramebuffer;
-(BOOL)hasRetinaDisplay;
-(GLint)getFramebuffer;

@end
