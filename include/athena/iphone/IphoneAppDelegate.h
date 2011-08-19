#import <UIKit/UIKit.h>

@interface IphoneAppDelegate : NSObject <UIApplicationDelegate> 
{
    UIViewController*       m_mainViewController;
}

@property (nonatomic, retain) IBOutlet UIWindow *window;

@end
