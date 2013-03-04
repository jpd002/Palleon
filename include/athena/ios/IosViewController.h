#import <UIKit/UIKit.h>
#import "athena/Application.h"

@interface IosViewController : UIViewController
{
	BOOL						animating;
	NSInteger					animationFrameInterval;
	CADisplayLink*				displayLink;

	UIActivityIndicatorView*	m_spinner;
	
	Athena::CApplication*		m_application;
	
	CFAbsoluteTime				m_currentTime;
	float						m_frameCounterTime;
	unsigned int				m_currentFrameCount;
	bool						m_isPortraitOrientation;
}

@property (readonly, nonatomic, getter=isAnimating) BOOL animating;
@property (nonatomic) NSInteger animationFrameInterval;

- (void)startAnimation;
- (void)stopAnimation;

@end
