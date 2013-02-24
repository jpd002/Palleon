#import <UIKit/UIKit.h>
#import "IphoneViewController.h"

@interface IphoneAppDelegate : NSObject <UIApplicationDelegate> 
{
	IphoneViewController*		m_mainViewController;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;

@end
