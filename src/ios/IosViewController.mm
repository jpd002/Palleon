#import <QuartzCore/QuartzCore.h>

//#define USE_METAL

#import "palleon/ios/IosViewController.h"
#import "palleon/ios/IosResourceManager.h"
#import "palleon/ios/IosAudioManager.h"
#import "palleon/ConfigManager.h"
#ifdef USE_METAL
#import "palleon/ios/MetalView.h"
#import "palleon/ios/MetalGraphicDevice.h"
#else
#import "palleon/ios/EAGLView.h"
#import "palleon/ios/IosGraphicDevice.h"
#endif
#include <mach/mach.h>
#include <mach/mach_time.h>
#include <MobileCoreServices/MobileCoreServices.h>

#define PREFERENCE_SCREEN_ORIENTATION_PORTRAIT	("screen.orientation.portrait")

using namespace Palleon;

@interface IosViewController ()
@property (nonatomic, assign) CADisplayLink *displayLink;
@end

@implementation IosViewController

@synthesize animating, displayLink;

-(id)init
{
	self = [super init];
	if (self)
	{
		m_isPortraitOrientation = false;
		
		CGRect screenBounds = [[UIScreen mainScreen] bounds];
		
#ifdef USE_METAL
		MetalView* view = [[MetalView alloc] initWithFrame: screenBounds];
		self.view = view;
		[view release];
#else
		EAGLView* view = [[EAGLView alloc] initWithFrame: screenBounds];
		self.view = view;
		[view release];
#endif
		
		m_spinner = [[UIActivityIndicatorView alloc] initWithActivityIndicatorStyle: UIActivityIndicatorViewStyleWhiteLarge];
		[m_spinner setCenter: CGPointMake(480 / 2, 320 / 2)];
		[self.view addSubview: m_spinner];
		//[m_spinner startAnimating];
		
		m_currentFrameCount = 0;
		m_frameCounterTime = 0;
	}
	
	return self;
}

-(BOOL)prefersStatusBarHidden
{
	return YES;
}

-(void)viewWillAppear:(BOOL)animated
{
	CIosResourceManager::CreateInstance();
	CIosAudioManager::CreateInstance();
	CConfigManager::CreateInstance();
	CConfigManager::GetInstance().GetConfig().RegisterPreferenceBoolean(PREFERENCE_SCREEN_ORIENTATION_PORTRAIT, false);

	CGRect screenBounds = [[UIScreen mainScreen] bounds];
	m_isPortraitOrientation = CConfigManager::GetInstance().GetConfig().GetPreferenceBoolean(PREFERENCE_SCREEN_ORIENTATION_PORTRAIT);
	
#ifdef USE_METAL
	auto view = (MetalView*)self.view;
	CMetalGraphicDevice::CreateInstance(view);
#else
	auto view = (EAGLView*)self.view;
	[view prepareContext];
	bool hasRetinaDisplay = [view hasRetinaDisplay];
	CIosGraphicDevice::CreateInstance(hasRetinaDisplay, CVector2(screenBounds.size.width, screenBounds.size.height));
#endif

	animating = FALSE;
	animationFrameInterval = 1;
	self.displayLink = nil;

	[self startAnimation];
	
	[super viewWillAppear:animated];
}

-(void)viewDidAppear: (BOOL)animated
{
#ifndef USE_METAL
	EAGLView* glView = (EAGLView*)self.view;
	[glView setFramebuffer];
#endif
	if(!m_application)
	{
		m_application = CreateApplication();
	}
}

-(void)dealloc
{
	[self stopAnimation];
	
	delete m_application;
	CIosAudioManager::DestroyInstance();
	CIosResourceManager::DestroyInstance();
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
	
#ifdef USE_METAL
	m_application->Update(static_cast<float>(deltaTime));
	CGraphicDevice::GetInstance().Draw();
#else
	EAGLView* glView = (EAGLView*)self.view;
	[glView setFramebuffer];
	m_application->Update(static_cast<float>(deltaTime));
	static_cast<CGlEsGraphicDevice&>(CGraphicDevice::GetInstance()).SetMainFramebuffer([glView getFramebuffer]);
	CGraphicDevice::GetInstance().Draw();
	[glView presentFramebuffer];
	
	m_currentFrameCount++;
	m_frameCounterTime += deltaTime;
	if(m_frameCounterTime > 1)
	{
		float frameRate = static_cast<float>(m_currentFrameCount) / m_frameCounterTime;
		m_frameCounterTime = 0;
		m_currentFrameCount = 0;
		static_cast<CGlEsGraphicDevice&>(CGraphicDevice::GetInstance()).SetFrameRate(frameRate);
	}
#endif
}

-(NSUInteger)supportedInterfaceOrientations
{
	if(m_isPortraitOrientation)
	{
		return UIInterfaceOrientationMaskPortrait;
	}
	else
	{
		return UIInterfaceOrientationMaskLandscape;
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
