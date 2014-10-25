#import <QuartzCore/QuartzCore.h>
#import "palleon/ios/EAGLView.h"

//#define USE_MULTISAMPLE

@interface EAGLView (PrivateMethods)
- (void)createFramebuffer;
- (void)deleteFramebuffer;
@end

@implementation EAGLView

@dynamic context;

// You must implement this method
+(Class)layerClass
{
	return [CAEAGLLayer class];
}

-(id)initWithFrame: (CGRect)frame
{
	self = [super initWithFrame: frame];
	if(self)
	{
		defaultFramebuffer = 0;
		colorRenderbuffer = 0;
		depthRenderbuffer = 0;
		
		msFramebuffer = 0;
		msColorRenderbuffer = 0;
		msDepthRenderbuffer = 0;
		
		hasRetinaDisplay = NO;
		if([[UIScreen mainScreen] respondsToSelector: NSSelectorFromString(@"scale")])
		{
			if([self respondsToSelector: NSSelectorFromString(@"contentScaleFactor")])
			{
				float scale = [[UIScreen mainScreen] scale];
				self.contentScaleFactor = scale;
				if(scale == 2.0f)
				{
					hasRetinaDisplay = YES;
				}
			}
		}
		
		CAEAGLLayer *eaglLayer = (CAEAGLLayer *)self.layer;
		
		eaglLayer.opaque = TRUE;
		eaglLayer.drawableProperties = [NSDictionary dictionaryWithObjectsAndKeys:
										[NSNumber numberWithBool:FALSE], kEAGLDrawablePropertyRetainedBacking,
										kEAGLColorFormatRGBA8, kEAGLDrawablePropertyColorFormat,
										nil];
		
		EAGLContext *aContext = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
		
		if (!aContext)
		{
			NSLog(@"Failed to create ES context");
		}
		else if (![EAGLContext setCurrentContext:aContext])
		{
			NSLog(@"Failed to set ES context current");
		}
		
		self.context = aContext;
		[aContext release];
		
		[EAGLContext setCurrentContext: self.context];
		[self createFramebuffer];
#ifdef USE_MULTISAMPLE
		[self createMsFramebuffer];
#endif
	}
	
	return self;
}

-(void)dealloc
{
	[self deleteFramebuffer];

	// Tear down context.
	if ([EAGLContext currentContext] == context)
	{
		[EAGLContext setCurrentContext:nil];
	}
	[context release];
	
	[super dealloc];
}

-(EAGLContext*)context
{
	return context;
}

-(void)setContext: (EAGLContext*)newContext
{
	if (context != newContext)
	{
		[self deleteFramebuffer];
		
		[context release];
		context = [newContext retain];
		
		[EAGLContext setCurrentContext:nil];
	}
}

-(void)createFramebuffer
{
	if(!context) return;
	
	assert(defaultFramebuffer == 0);
	
	// Create default framebuffer object.
	glGenFramebuffers(1, &defaultFramebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebuffer);
		
	glGenRenderbuffers(1, &depthRenderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
		
	// Create color render buffer and allocate backing store.
	glGenRenderbuffers(1, &colorRenderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer);
	[context renderbufferStorage:GL_RENDERBUFFER fromDrawable:(CAEAGLLayer *)self.layer];
	glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &framebufferWidth);
	glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &framebufferHeight);

	glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8_OES, framebufferWidth, framebufferHeight);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorRenderbuffer);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthRenderbuffer);

	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		NSLog(@"Failed to make complete framebuffer object %x", glCheckFramebufferStatus(GL_FRAMEBUFFER));
		assert(false);
	}
}

-(void)deleteFramebuffer
{
	if(!context) return;
	
	if(defaultFramebuffer)
	{
		glDeleteFramebuffers(1, &defaultFramebuffer);
		defaultFramebuffer = 0;
	}
		
	if(colorRenderbuffer)
	{
		glDeleteRenderbuffers(1, &colorRenderbuffer);
		colorRenderbuffer = 0;
	}
		
	if(depthRenderbuffer)
	{
		glDeleteRenderbuffers(1, &depthRenderbuffer);
		depthRenderbuffer = 0;
	}
}

-(void)createMsFramebuffer
{
	assert(msFramebuffer == 0);
	
	glGenFramebuffers(1, &msFramebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, msFramebuffer);

	glGenRenderbuffers(1, &msColorRenderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, msColorRenderbuffer);
	glRenderbufferStorageMultisampleAPPLE(GL_RENDERBUFFER, 2, GL_RGBA8_OES, framebufferWidth, framebufferHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, msColorRenderbuffer);

	glGenRenderbuffers(1, &msDepthRenderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, msDepthRenderbuffer);
	glRenderbufferStorageMultisampleAPPLE(GL_RENDERBUFFER, 2, GL_DEPTH24_STENCIL8_OES, framebufferWidth, framebufferHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, msDepthRenderbuffer);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, msDepthRenderbuffer);

	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		NSLog(@"Failed to make complete framebuffer object %x", glCheckFramebufferStatus(GL_FRAMEBUFFER));
		assert(false);
	}
}

-(void)deleteMsFramebuffer
{
	if(msColorRenderbuffer != 0)
	{
		glDeleteRenderbuffers(1, &msColorRenderbuffer);
		msColorRenderbuffer = 0;
	}
	
	if(msDepthRenderbuffer != 0)
	{
		glDeleteRenderbuffers(1, &msDepthRenderbuffer);
		msDepthRenderbuffer = 0;
	}
	
	if(msFramebuffer != 0)
	{
		glDeleteFramebuffers(1, &msFramebuffer);
		msFramebuffer = 0;
	}
}

-(BOOL)presentFramebuffer
{
	BOOL success = FALSE;
	
	if(context)
	{
		[EAGLContext setCurrentContext: context];
		
#ifdef USE_MULTISAMPLE
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER_APPLE, defaultFramebuffer);
		glBindFramebuffer(GL_READ_FRAMEBUFFER_APPLE, msFramebuffer);
		glResolveMultisampleFramebufferAPPLE();
		CHECKGLERROR();
		
		const GLenum discards[]  = { GL_COLOR_ATTACHMENT0, GL_DEPTH_ATTACHMENT, GL_STENCIL_ATTACHMENT };
		glDiscardFramebufferEXT(GL_READ_FRAMEBUFFER_APPLE, 3, discards);
		CHECKGLERROR();
#endif

		glBindRenderbuffer(GL_RENDERBUFFER, colorRenderbuffer);
		CHECKGLERROR();
		
		success = [context presentRenderbuffer: GL_RENDERBUFFER];
	}
	
	return success;
}

-(BOOL)hasRetinaDisplay
{
	return hasRetinaDisplay;
}

-(GLint)getFramebuffer
{
#ifdef USE_MULTISAMPLE
	return msFramebuffer;
#else
	return defaultFramebuffer;
#endif
}

@end
