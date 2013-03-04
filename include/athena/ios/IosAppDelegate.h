#import <UIKit/UIKit.h>
#import "IosViewController.h"

@interface IphoneAppDelegate : NSObject <UIApplicationDelegate> 
{
	IosViewController*		m_mainViewController;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;

@end
