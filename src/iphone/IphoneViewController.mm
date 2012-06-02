#import <QuartzCore/QuartzCore.h>

#import "athena/iphone/IphoneViewController.h"
#import "athena/iphone/IphoneGraphicDevice.h"
#import "athena/iphone/IphoneResourceManager.h"
#import "athena/ConfigManager.h"
#import "EAGLView.h"
#include <mach/mach.h>
#include <mach/mach_time.h>
#include <MobileCoreServices/MobileCoreServices.h>

#define PREFERENCE_SCREEN_ORIENTATION_PORTRAIT	("screen.orientation.portrait")

using namespace Athena;

@interface IphoneViewController ()
@property (nonatomic, assign) CADisplayLink *displayLink;
@end

@implementation IphoneViewController

@synthesize animating, displayLink;

-(id)init
{
	self = [super init];
	if (self)
	{
		m_isPortraitOrientation = false;
		
		CGRect screenBounds = [[UIScreen mainScreen] bounds];
		
		EAGLView* view = [[EAGLView alloc] initWithFrame: screenBounds];
		self.view = view;
		[view release];
		
		m_spinner = [[UIActivityIndicatorView alloc] initWithActivityIndicatorStyle: UIActivityIndicatorViewStyleWhiteLarge];
		[m_spinner setCenter: CGPointMake(480 / 2, 320 / 2)];
		[self.view addSubview: m_spinner];
		//[m_spinner startAnimating];
		
		m_currentFrameCount = 0;
		m_frameCounterTime = 0;
	}
	
	return self;
}

-(void)viewWillAppear:(BOOL)animated
{
	[(EAGLView *)self.view prepareContext];
	
	CGRect screenBounds = [[UIScreen mainScreen] bounds];

	bool hasRetinaDisplay = [(EAGLView *)self.view hasRetinaDisplay];
	
	CIphoneResourceManager::CreateInstance();
	CConfigManager::CreateInstance();
	CConfigManager::GetInstance().GetConfig().RegisterPreferenceBoolean(PREFERENCE_SCREEN_ORIENTATION_PORTRAIT, false);
	
	m_isPortraitOrientation = CConfigManager::GetInstance().GetConfig().GetPreferenceBoolean(PREFERENCE_SCREEN_ORIENTATION_PORTRAIT);
	
	if(m_isPortraitOrientation)
	{
		CIphoneGraphicDevice::CreateInstance(hasRetinaDisplay, CVector2(screenBounds.size.width, screenBounds.size.height));
	}
	else
	{
		CIphoneGraphicDevice::CreateInstance(hasRetinaDisplay, CVector2(screenBounds.size.height, screenBounds.size.width));
	}

	assert(m_application == NULL);
	m_application = CreateApplication();

	animating = FALSE;
	animationFrameInterval = 1;
	self.displayLink = nil;

	[self startAnimation];
	
	[super viewWillAppear:animated];
}

-(void)dealloc
{
	[self stopAnimation];
	
	delete m_application;
	CIphoneResourceManager::DestroyInstance();
	[super dealloc];
}

-(NSInteger)animationFrameInterval
{
	return animationFrameInterval;
}

-(void)setAnimationFrameInterval:(NSInteger)frameInterval
{
	/*
	 Frame interval defines how many display frames must pass between each time the display link fires.
	 The display link will only fire 30 times a second when the frame internal is two on a display that refreshes 60 times a second. The default frame interval setting of one will fire 60 times a second when the display refreshes at 60 times a second. A frame interval setting of less than one results in undefined behavior.
	 */
	if (frameInterval >= 1)
	{
		animationFrameInterval = frameInterval;
		
		if (animating)
		{
			[self stopAnimation];
			[self startAnimation];
		}
	}
}

-(void)startAnimation
{
	if (!animating)
	{
		m_currentTime = CFAbsoluteTimeGetCurrent();

		CADisplayLink *aDisplayLink = [CADisplayLink displayLinkWithTarget:self selector:@selector(drawFrame)];
		[aDisplayLink setFrameInterval: animationFrameInterval];
		[aDisplayLink addToRunLoop:[NSRunLoop currentRunLoop] forMode:NSDefaultRunLoopMode];
		self.displayLink = aDisplayLink;
		
		animating = TRUE;
	}
}

-(void)stopAnimation
{
	if (animating)
	{
		[self.displayLink invalidate];
		self.displayLink = nil;
		animating = FALSE;
	}
}

-(void)drawFrame
{
	CFAbsoluteTime nextTime = CFAbsoluteTimeGetCurrent();
	CFAbsoluteTime deltaTime = nextTime - m_currentTime;
	m_currentTime = nextTime;
	[(EAGLView *)self.view setFramebuffer];
	m_application->Update(static_cast<float>(deltaTime));
	CGraphicDevice::GetInstance().Draw();
	[(EAGLView *)self.view presentFramebuffer];
	
	m_currentFrameCount++;
	m_frameCounterTime += deltaTime;
	if(m_frameCounterTime > 1)
	{
		float frameRate = static_cast<float>(m_currentFrameCount) / m_frameCounterTime;
		m_frameCounterTime = 0;
		m_currentFrameCount = 0;
		static_cast<CIphoneGraphicDevice&>(CGraphicDevice::GetInstance()).SetFrameRate(frameRate);
	}
	
}

-(BOOL)shouldAutorotateToInterfaceOrientation: (UIInterfaceOrientation)toInterfaceOrientation
{
	if(m_isPortraitOrientation && toInterfaceOrientation == UIInterfaceOrientationPortrait)
	{
		return YES;
	}
	else if(!m_isPortraitOrientation &&
			(
			 toInterfaceOrientation == UIInterfaceOrientationLandscapeLeft ||
			 toInterfaceOrientation == UIInterfaceOrientationLandscapeRight
			 )
			)
	{
		return YES;
	}
	else
	{
		return NO;
	}
}

-(void)touchesBegan: (NSSet*)touches withEvent: (UIEvent*)event
{
	NSEnumerator* touchEnumerator = [touches objectEnumerator];
	UITouch* touch = nil;
	while((touch = [touchEnumerator nextObject]))
	{
		CGPoint position = [touch locationInView: self.view];
		m_application->NotifyMouseMove(position.x, position.y);
		m_application->NotifyMouseDown();
	}
}

-(void)touchesMoved: (NSSet*)touches withEvent: (UIEvent*)event
{
	NSEnumerator* touchEnumerator = [touches objectEnumerator];
	UITouch* touch = nil;
	while((touch = [touchEnumerator nextObject]))
	{
		CGPoint position = [touch locationInView: self.view];
		m_application->NotifyMouseMove(position.x, position.y);
	}
}

-(void)touchesEnded: (NSSet*)touches withEvent: (UIEvent*)event
{
	NSEnumerator* touchEnumerator = [touches objectEnumerator];
	UITouch* touch = nil;
	while((touch = [touchEnumerator nextObject]))
	{
		m_application->NotifyMouseUp();
	}
}

@end
