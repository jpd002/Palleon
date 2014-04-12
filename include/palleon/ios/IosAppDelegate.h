#import <UIKit/UIKit.h>
#import "IosViewController.h"

@interface IosAppDelegate : NSObject <UIApplicationDelegate>
{
	IosViewController*		m_mainViewController;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;

@end
